import { readFileSync, writeFileSync, readdirSync } from 'fs';
import { join, dirname, basename } from 'path';
import { fileURLToPath } from 'url';
import { marked } from 'marked';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const contentDir = join(__dirname, '../../content');
const outputDir = join(__dirname, '../../');

// Navigation items - manually configured
const navItems = [
  { title: 'About', href: '/', page: 'about' },
  { title: 'Materials', href: '/materials-list.html', page: 'materials-list' },
  { title: 'Build', href: '/build.html', page: 'build' },
  { title: 'Code Documentation', href: '/documentation.html', page: 'documentation' },
  { title: 'CSV Viewer', href: '/csv-viewer/', page: null },
];

function createHeader(currentPage) {
  const navLinks = navItems
    .map(item => {
      const isActive = item.page === currentPage;
      const activeClass = isActive ? 'text-blue-600 font-medium' : 'text-slate-600 hover:text-slate-900';
      return `<a href="${item.href}" class="${activeClass}">${item.title}</a>`;
    })
    .join('\n          ');

  return `
    <header class="border-b border-slate-200 bg-white">
      <div class="mx-auto max-w-5xl px-4 py-4">
        <nav class="flex items-center gap-6">
          <div class="text-lg font-semibold text-slate-900">OpenTholoMetri</div>
          ${navLinks}
        </nav>
      </div>
    </header>`;
}

function createTemplate(title, bodyHtml, currentPage) {
  return `<!doctype html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>${title} - OpenTholoMetri</title>
    <link rel="icon" href="/assets/favicon.png" />
    <script src="https://cdn.tailwindcss.com"></script>
    <style>
      .prose h1 {
        font-size: 1.875rem;
        font-weight: 700;
        margin-bottom: 1rem;
        margin-top: 2rem;
        color: #0f172a;
      }
      .prose h2 {
        font-size: 1.5rem;
        font-weight: 600;
        margin-bottom: 0.75rem;
        margin-top: 1.5rem;
        color: #0f172a;
      }
      .prose h3 {
        font-size: 1.25rem;
        font-weight: 500;
        margin-bottom: 0.5rem;
        margin-top: 1rem;
        color: #0f172a;
      }
      .prose p {
        margin-bottom: 1rem;
        color: #334155;
        line-height: 1.625;
      }
      .prose ul, .prose ol {
        margin-bottom: 1rem;
        margin-left: 1.5rem;
        color: #334155;
      }
      .prose ul {
        list-style-type: disc;
      }
      .prose ol {
        list-style-type: decimal;
      }
      .prose li {
        margin-bottom: 0.5rem;
      }
      .prose strong {
        font-weight: 600;
        color: #0f172a;
      }
      .prose a {
        color: #2563eb;
        text-decoration: none;
      }
      .prose a:hover {
        text-decoration: underline;
      }
      .prose code {
        background-color: #f1f5f9;
        padding: 0.125rem 0.375rem;
        border-radius: 0.25rem;
        font-size: 0.875rem;
        font-family: ui-monospace, monospace;
        color: #1e293b;
      }
      .prose pre {
        background-color: #f1f5f9;
        padding: 1rem;
        border-radius: 0.5rem;
        overflow-x: auto;
        margin-bottom: 1rem;
      }
      .prose pre code {
        background-color: transparent;
        padding: 0;
      }
      .prose table {
        width: 100%;
        border-collapse: collapse;
        margin-bottom: 1rem;
      }
      .prose th,
      .prose td {
        border: 1px solid #e2e8f0;
        padding: 0.5rem 0.75rem;
        vertical-align: top;
        color: #334155;
      }
      .prose th {
        background-color: #f8fafc;
        font-weight: 600;
        color: #0f172a;
        text-align: left;
      }
      .prose tbody tr:nth-child(even) {
        background-color: #f8fafc;
      }
    </style>
  </head>
  <body class="min-h-screen bg-slate-50">
    ${createHeader(currentPage)}
    <main class="mx-auto max-w-5xl px-4 py-8">
      <article class="prose max-w-none">
        ${bodyHtml}
      </article>
    </main>
  </body>
</html>`;
}

function buildPages() {
  console.log('Building static pages from markdown...\n');

  const files = readdirSync(contentDir).filter(f => f.endsWith('.md'));

  for (const file of files) {
    const mdPath = join(contentDir, file);
    const content = readFileSync(mdPath, 'utf-8');
    
    // Convert markdown to HTML
    const html = marked(content);
    
    // Extract title from first h1
    const titleMatch = content.match(/^#\s+(.+)$/m);
    const title = titleMatch ? titleMatch[1] : 'OpenTholoMetri';
    
    // Determine output filename and page identifier
    const pageName = basename(file, '.md');
    let outputFile;
    if (pageName === 'about') {
      outputFile = join(outputDir, 'index.html');
    } else {
      outputFile = join(outputDir, `${pageName}.html`);
    }
    
    // Create full HTML page
    const fullHtml = createTemplate(title, html, pageName);
    
    // Write to output
    writeFileSync(outputFile, fullHtml, 'utf-8');
    console.log(`✓ Built ${file} -> ${basename(outputFile)}`);
  }

  console.log('\n✓ Static site generation complete!');
}

buildPages();
