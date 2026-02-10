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
  { title: 'Documentation', href: '/documentation.html', page: 'documentation' },
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
    <script src="https://cdn.tailwindcss.com"></script>
    <style>
      .prose h1 { @apply text-3xl font-bold mb-4 mt-8 text-slate-900; }
      .prose h2 { @apply text-2xl font-semibold mb-3 mt-6 text-slate-900; }
      .prose h3 { @apply text-xl font-medium mb-2 mt-4 text-slate-900; }
      .prose p { @apply mb-4 text-slate-700 leading-relaxed; }
      .prose ul { @apply mb-4 ml-6 list-disc text-slate-700; }
      .prose li { @apply mb-2; }
      .prose strong { @apply font-semibold text-slate-900; }
      .prose a { @apply text-blue-600 hover:underline; }
      .prose code { @apply bg-slate-100 px-1.5 py-0.5 rounded text-sm font-mono text-slate-800; }
      .prose pre { @apply bg-slate-100 p-4 rounded-lg overflow-x-auto mb-4; }
      .prose pre code { @apply bg-transparent p-0; }
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
