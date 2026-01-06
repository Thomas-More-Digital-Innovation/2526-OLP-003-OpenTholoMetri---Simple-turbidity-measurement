type Row = Record<string, unknown>;

type ParseResult = {
  rows: Row[];
  fields: string[];
};

function byId<T extends HTMLElement>(id: string): T {
  const el = document.getElementById(id);
  if (!el) throw new Error(`Missing element #${id}`);
  return el as T;
}

function setText(id: string, value: string): void {
  byId<HTMLElement>(id).textContent = value;
}

function showError(message: string): void {
  const el = byId<HTMLDivElement>("error");
  el.textContent = message;
  el.classList.remove("hidden");
}

function clearError(): void {
  const el = byId<HTMLDivElement>("error");
  el.textContent = "";
  el.classList.add("hidden");
}

function parseTimestampLocal(value: unknown): Date | null {
  if (typeof value !== "string") return null;
  const trimmed = value.trim();
  // Expected: YYYY-MM-DD HH:MM:SS
  const match = /^(\d{4})-(\d{2})-(\d{2})[ T](\d{2}):(\d{2}):(\d{2})$/.exec(trimmed);
  if (!match) return null;

  const year = Number(match[1]);
  const month = Number(match[2]);
  const day = Number(match[3]);
  const hour = Number(match[4]);
  const minute = Number(match[5]);
  const second = Number(match[6]);

  if (
    !Number.isFinite(year) ||
    !Number.isFinite(month) ||
    !Number.isFinite(day) ||
    !Number.isFinite(hour) ||
    !Number.isFinite(minute) ||
    !Number.isFinite(second)
  ) {
    return null;
  }

  const d = new Date(year, month - 1, day, hour, minute, second);
  if (Number.isNaN(d.getTime())) return null;
  return d;
}

function timestampParseRate(field: string, rows: Row[]): number {
  const sampleRows = rows.slice(0, Math.min(20, rows.length));
  let ok = 0;
  let total = 0;

  for (const row of sampleRows) {
    const v = row[field];
    if (v == null || v === "") continue;
    total++;
    if (parseTimestampLocal(v)) ok++;
  }

  if (total === 0) return 0;
  return ok / total;
}

function detectTimestampColumn(fields: string[], rows: Row[]): string | null {
  const preferred = fields.find((f) => f.toLowerCase().includes("timestamp"));
  if (preferred && timestampParseRate(preferred, rows) >= 0.4) return preferred;

  const timeLike = fields.filter((f) => /time|date/i.test(f));
  for (const field of timeLike) {
    if (timestampParseRate(field, rows) >= 0.7) return field;
  }

  for (const field of fields) {
    if (timestampParseRate(field, rows) >= 0.7) return field;
  }

  return null;
}

function isMostlyNumeric(field: string, rows: Row[]): boolean {
  const sampleRows = rows.slice(0, Math.min(40, rows.length));
  let total = 0;
  let numeric = 0;

  for (const row of sampleRows) {
    const v = row[field];
    if (v == null || v === "") continue;
    total++;

    if (typeof v === "number" && Number.isFinite(v)) {
      numeric++;
      continue;
    }

    if (typeof v === "string") {
      const n = Number(v);
      if (Number.isFinite(n)) numeric++;
    }
  }

  if (total === 0) return false;
  return numeric / total >= 0.8;
}

function coerceNumber(value: unknown): number | null {
  if (typeof value === "number") return Number.isFinite(value) ? value : null;
  if (typeof value === "string") {
    const n = Number(value);
    return Number.isFinite(n) ? n : null;
  }
  return null;
}

function toCsvParseResult(csvText: string): Promise<ParseResult> {
  return new Promise((resolve, reject) => {
    Papa.parse(csvText, {
      header: true,
      skipEmptyLines: true,
      dynamicTyping: true,
      complete: (results: any) => {
        const fields: string[] = results?.meta?.fields ?? [];
        const rows: Row[] = Array.isArray(results?.data) ? results.data : [];

        if (results?.errors?.length) {
          const first = results.errors[0];
          reject(new Error(`CSV parse error: ${first?.message ?? "Unknown error"}`));
          return;
        }

        if (!fields.length) {
          reject(new Error("No headers detected. Make sure the first row contains column names."));
          return;
        }

        resolve({ rows, fields });
      },
      error: (err: any) => reject(err),
    });
  });
}

function fillSelect(select: HTMLSelectElement, options: string[], selected: string | null): void {
  select.innerHTML = "";
  for (const opt of options) {
    const optionEl = document.createElement("option");
    optionEl.value = opt;
    optionEl.textContent = opt;
    if (selected && opt === selected) optionEl.selected = true;
    select.appendChild(optionEl);
  }
}

function formatCellValue(value: unknown): string {
  if (value == null) return "";
  if (value instanceof Date) return value.toISOString();
  if (typeof value === "object") {
    try {
      return JSON.stringify(value);
    } catch {
      return "[object]";
    }
  }
  if (typeof value === "string") return value;
  if (typeof value === "number" || typeof value === "boolean" || typeof value === "bigint") return String(value);
  if (typeof value === "symbol") return value.toString();
  if (typeof value === "function") return "[function]";
  return "";
}

function renderTable(fields: string[], rows: Row[]): void {
  const head = byId<HTMLTableSectionElement>("tableHead");
  const body = byId<HTMLTableSectionElement>("tableBody");
  head.innerHTML = "";
  body.innerHTML = "";

  const trh = document.createElement("tr");
  for (const f of fields) {
    const th = document.createElement("th");
    th.className = "border-b border-slate-200 px-3 py-2 text-xs font-semibold text-slate-600";
    th.textContent = f;
    trh.appendChild(th);
  }
  head.appendChild(trh);

  const preview = rows.slice(0, 50);
  for (const row of preview) {
    const tr = document.createElement("tr");
    for (const f of fields) {
      const td = document.createElement("td");
      td.className = "border-b border-slate-100 px-3 py-2 align-top text-xs text-slate-700";
      const v = row[f];
      td.textContent = formatCellValue(v);
      tr.appendChild(td);
    }
    body.appendChild(tr);
  }
}

function renderSummary(fields: string[], rows: Row[], timestampField: string | null): void {
  setText("summaryRows", String(rows.length));
  setText("summaryCols", String(fields.length));

  if (!timestampField) {
    setText("summaryStart", "—");
    setText("summaryEnd", "—");
    return;
  }

  const dates: Date[] = [];
  for (const row of rows) {
    const d = parseTimestampLocal(row[timestampField]);
    if (d) dates.push(d);
  }

  dates.sort((a, b) => a.getTime() - b.getTime());
  const start = dates[0];
  const end = dates.at(-1);

  setText("summaryStart", start ? start.toLocaleString() : "—");
  setText("summaryEnd", end ? end.toLocaleString() : "—");
}

function plot(fields: string[], rows: Row[], xField: string, yField: string): void {
  const xIsTimestamp = !!parseTimestampLocal(rows.find((r) => r[xField] != null)?.[xField]);

  const x: (Date | number)[] = [];
  const y: number[] = [];

  for (let i = 0; i < rows.length; i++) {
    const row = rows[i];

    const yVal = coerceNumber(row[yField]);
    if (yVal == null) continue;

    if (xIsTimestamp) {
      const d = parseTimestampLocal(row[xField]);
      if (!d) continue;
      x.push(d);
      y.push(yVal);
    } else {
      const xNum = coerceNumber(row[xField]);
      x.push(xNum ?? i);
      y.push(yVal);
    }
  }

  const trace = {
    x,
    y,
    type: "scatter",
    mode: "lines+markers",
    marker: { size: 4 },
    name: yField,
  };

  const layout = {
    margin: { l: 50, r: 20, t: 10, b: 45 },
    xaxis: { title: xField },
    yaxis: { title: yField },
    showlegend: false,
  };

  Plotly.react("plot", [trace], layout, { responsive: true });
}

async function readFileAsText(file: File): Promise<string> {
  return await file.text();
}

function enableJsonDownload(rows: Row[]): void {
  const button = byId<HTMLButtonElement>("downloadJson");
  button.classList.remove("hidden");

  button.onclick = () => {
    const blob = new Blob([JSON.stringify(rows, null, 2)], { type: "application/json" });
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = "opentholometri.json";
    document.body.appendChild(a);
    a.click();
    a.remove();
    URL.revokeObjectURL(url);
  };
}

async function handleCsvFile(file: File): Promise<void> {
  clearError();
  setText("status", `Loading ${file.name}…`);

  const text = await readFileAsText(file);
  const { rows, fields } = await toCsvParseResult(text);

  const timestampField = detectTimestampColumn(fields, rows);
  const firstNumericField = fields.find((f) => isMostlyNumeric(f, rows));

  const xDefault = timestampField ?? fields[0];
  const yDefault = firstNumericField ?? fields.find((f) => f !== xDefault) ?? fields[0];

  const xSelect = byId<HTMLSelectElement>("xSelect");
  const ySelect = byId<HTMLSelectElement>("ySelect");

  fillSelect(xSelect, fields, xDefault);
  fillSelect(ySelect, fields, yDefault);

  renderSummary(fields, rows, timestampField);
  renderTable(fields, rows);
  enableJsonDownload(rows);

  const updatePlot = () => plot(fields, rows, xSelect.value, ySelect.value);
  xSelect.onchange = updatePlot;
  ySelect.onchange = updatePlot;

  updatePlot();
  setText("status", `Loaded ${rows.length} rows.`);
}

function main(): void {
  const fileInput = byId<HTMLInputElement>("fileInput");
  const dropZone = byId<HTMLDivElement>("dropZone");

  fileInput.addEventListener("change", async () => {
    const file = fileInput.files?.[0];
    if (!file) return;

    try {
      await handleCsvFile(file);
    } catch (e) {
      const message = e instanceof Error ? e.message : String(e);
      showError(message);
      setText("status", "");
    }
  });

  const setDragActive = () => dropZone.classList.add("border-slate-400", "bg-white");
  const setDragInactive = () => dropZone.classList.remove("border-slate-400", "bg-white");

  dropZone.addEventListener("dragover", (e) => {
    e.preventDefault();
    setDragActive();
  });

  dropZone.addEventListener("dragleave", () => setDragInactive());

  dropZone.addEventListener("drop", async (e) => {
    e.preventDefault();
    setDragInactive();

    const file = e.dataTransfer?.files?.[0];
    if (!file) return;

    try {
      await handleCsvFile(file);
    } catch (err) {
      const message = err instanceof Error ? err.message : String(err);
      showError(message);
      setText("status", "");
    }
  });
}

main();
