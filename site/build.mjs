#!/usr/bin/env node
// Build a static catalog from loom.json and each unit README.
// No runtime dependencies and no network access.

import {execFileSync} from "node:child_process";
import fs from "node:fs";
import path from "node:path";
import {fileURLToPath} from "node:url";

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "..");
const out = path.resolve(root, process.argv.includes("--out")
  ? process.argv[process.argv.indexOf("--out") + 1]
  : "site/dist");
const manifest = JSON.parse(fs.readFileSync(path.join(root, "loom.json"), "utf8"));
const lockPath = path.join(root, "loom.lock");
const lock = fs.existsSync(lockPath)
  ? JSON.parse(fs.readFileSync(lockPath, "utf8"))
  : {ref: git(["describe", "--tags", "--abbrev=0"]) || "main", commit: git(["rev-parse", "HEAD"])};
const source = "https://git.pcbsmt.me/rh/emb-loom.git";
const logo = `<svg viewBox="0 0 32 32" width="22" height="22" aria-hidden="true"><path d="M16 3c-4.4 0-8 3.6-8 8v3H6v4h2v11h4V18h3v11h4V18h3v11h4V14c0-4.4-3.6-8-8-8h-2z" fill="currentColor"/><circle cx="16" cy="8" r="2.2" fill="var(--paper)"/></svg>`;
const favicon = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 32 32"><rect width="32" height="32" rx="7" fill="#1d1b17"/><path d="M16 5c-4 0-7 3-7 7v2H7v4h2v9h4v-9h2v9h4v-9h2v9h4v-9h2v-4h-2v-2c0-4-3-7-7-7h-2z" fill="#f4a261"/><circle cx="16" cy="10" r="1.8" fill="#1d1b17"/></svg>`;
const units = manifest.units.map(unit => ({
  ...unit,
  readme: readReadme(unit.path),
  source_url: `${source.replace(/\.git$/, "")}/src/tag/${encodeURIComponent(lock.ref)}/${unit.path}`,
}));

fs.rmSync(out, {recursive: true, force: true});
fs.mkdirSync(path.join(out, "units"), {recursive: true});
fs.writeFileSync(path.join(out, "style.css"), fs.readFileSync(path.join(root, "site/style.css")));
fs.writeFileSync(path.join(out, "index.html"), page("Catalog", indexBody(units, lock), ""));
for (const unit of units) {
  fs.writeFileSync(path.join(out, "units", `${unit.id}.html`), page(unit.id, unitBody(unit, lock), "../"));
}
fs.writeFileSync(path.join(out, "contribute.html"), page("Contribute", contributeBody(), ""));
fs.writeFileSync(path.join(out, "index.json"), JSON.stringify({
  schema: 1,
  catalog: manifest.version,
  source,
  ref: lock.ref,
  commit: lock.commit,
  units: units.map(unit => ({
    id: unit.id, kind: unit.kind, version: unit.version, summary: unit.summary,
    tags: unit.tags, applications: unit.applications, compatibility: unit.compatibility,
    path: unit.path,
  })),
}, null, 2) + "\n");
console.log(`loom site: ${units.length} units -> ${out}`);

function readReadme(relative) {
  const file = path.join(root, relative, "README.md");
  return fs.existsSync(file) ? fs.readFileSync(file, "utf8") : "";
}

function git(args) {
  try {
    return execFileSync("git", ["-C", root, ...args], {encoding: "utf8"}).trim();
  } catch {
    return "";
  }
}

function page(title, body, prefix) {
  return `<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>${escape(title)} · loom</title>
<meta name="description" content="Small, tested C drivers and algorithms for embedded systems. Pinned by commit and SHA-256.">
<link rel="icon" href="data:image/svg+xml,${encodeURIComponent(favicon)}">
<link rel="stylesheet" href="${prefix}style.css">
</head>
<body>
<header class="site-header">
  <a class="brand" href="${prefix}index.html" aria-label="loom catalog">
    <span class="brand-mark">${logo}</span><span>loom</span>
  </a>
  <div class="search-wrap">
    <span class="search-icon">⌕</span>
    <input id="search" type="search" placeholder="Search packages, tags, applications" aria-label="Search packages" autocomplete="off">
    <kbd>/</kbd>
  </div>
  <nav class="nav">
    <a href="${prefix}index.html">Browse</a>
    <a href="${prefix}contribute.html">Contribute</a>
    <a href="#about">About</a>
    <a href="${escape(source)}" class="source-link">Source ↗</a>
    <span class="ref">${escape(lock.ref || "")}</span>
  </nav>
</header>
${body}
<footer class="footer">
  <div class="footer-inner">
    <div><strong>loom</strong> <span>${escape(manifest.version)}</span></div>
    <div>${units.length} packages · pinned by commit · no install scripts</div>
    <div><a href="${escape(source)}">Repository</a> · <a href="${prefix}index.json">index.json</a></div>
  </div>
</footer>
</body>
</html>`;
}

function indexBody(units, lock) {
  const tags = [...new Set(units.flatMap(unit => unit.tags || []))].sort();
  const operatorCount = units.filter(unit => unit.kind === "operator").length;
  const driverCount = units.length - operatorCount;
  return `<main>
<section class="hero">
  <div class="hero-copy">
    <p class="eyebrow">The embedded package registry</p>
    <h1>Drivers and algorithms,<br><em>ready to pin.</em></h1>
    <p class="lede">Small, tested C units for embedded systems. Each package has a contract, a README, host tests, a source commit, and a SHA-256.</p>
    <code class="hero-install">loom add median</code>
  </div>
  <div class="stats">
    <div><strong>${units.length}</strong><span>packages</span></div>
    <div><strong>${operatorCount}</strong><span>operators</span></div>
    <div><strong>${driverCount}</strong><span>drivers</span></div>
    <div><strong>C99</strong><span>no runtime</span></div>
  </div>
</section>

<section class="catalog" id="catalog">
  <aside class="filters">
    <div class="filter-block">
      <h2>Category</h2>
      <button class="filter is-active" data-kind="">All packages <span>${units.length}</span></button>
      <button class="filter" data-kind="operator">Operators <span>${operatorCount}</span></button>
      <button class="filter" data-kind="driver">Drivers <span>${driverCount}</span></button>
    </div>
    <div class="filter-block">
      <h2>Tags</h2>
      <div class="tag-cloud">
        ${tags.map(tag => `<button class="tag" data-tag="${escape(tag)}">${escape(tag)}</button>`).join("")}
      </div>
    </div>
    <div class="filter-block about" id="about">
      <h2>About</h2>
      <p>loom is a curated catalog, not a package manager service. Units are copied into a project and pinned in <code>loom.lock</code>.</p>
      <p>Source: <a href="${escape(source)}">git.pcbsmt.me/rh/emb-loom</a></p>
    </div>
  </aside>

  <div class="results">
    <div class="results-head">
      <h2 id="result-title">All packages</h2>
      <span id="count" class="count"></span>
    </div>
    <div class="package-list" id="grid">
      ${units.map(card).join("\n")}
    </div>
    <p class="empty" id="empty" hidden>No packages match that filter. <button id="clear">Clear filters</button></p>
  </div>
</section>
</main>
<script>
const search = document.getElementById('search');
const count = document.getElementById('count');
const empty = document.getElementById('empty');
const title = document.getElementById('result-title');
const params = new URLSearchParams(location.search);
if (params.get('q')) search.value = params.get('q');
const cards = [...document.querySelectorAll('.pkg')];
const filters = [...document.querySelectorAll('.filter[data-kind]')];
const tags = [...document.querySelectorAll('.tag[data-tag]')];
let kind = '';
let tag = '';
function apply() {
  const q = search.value.trim().toLowerCase();
  let shown = 0;
  for (const card of cards) {
    const ok = (!q || card.dataset.search.includes(q))
      && (!kind || card.dataset.kind === kind)
      && (!tag || card.dataset.tags.split(' ').includes(tag));
    card.hidden = !ok;
    if (ok) shown++;
  }
  count.textContent = shown + ' of ' + cards.length;
  title.textContent = kind === 'operator' ? 'Operators' : kind === 'driver' ? 'Drivers' : 'All packages';
  empty.hidden = shown !== 0;
  for (const button of filters) button.classList.toggle('is-active', button.dataset.kind === kind && !tag);
}
for (const button of filters) button.addEventListener('click', () => { kind = button.dataset.kind; tag = ''; apply(); });
for (const button of tags) button.addEventListener('click', () => {
  tag = tag === button.dataset.tag ? '' : button.dataset.tag;
  kind = '';
  search.value = '';
  apply();
});
document.getElementById('clear').addEventListener('click', () => {
  kind = ''; tag = ''; search.value = ''; apply();
});
search.addEventListener('input', apply);
document.addEventListener('keydown', event => {
  if (event.key === '/' && document.activeElement !== search) { event.preventDefault(); search.focus(); }
});
apply();
</script>`;
}

function card(unit) {
  const hay = [unit.id, unit.summary, ...(unit.tags || []), ...(unit.applications || [])].join(" ").toLowerCase();
  return `<article class="pkg" data-kind="${escape(unit.kind)}" data-tags="${escape((unit.tags || []).join(" "))}" data-search="${escape(hay)}">
  <div class="pkg-main">
    <div class="pkg-title">
      <a class="pkg-name" href="units/${escape(unit.id)}.html">${escape(unit.id)}</a>
      <span class="pkg-version">v${escape(unit.version)}</span>
      <span class="kind ${escape(unit.kind)}">${escape(unit.kind)}</span>
    </div>
    <p class="pkg-desc">${escape(unit.summary)}</p>
    <div class="pkg-tags">${(unit.tags || []).map(tag => `<span>${escape(tag)}</span>`).join("")}</div>
  </div>
  <div class="pkg-side">
    <div class="compat">${compat(unit.compatibility)}</div>
    <p class="pkg-apps">${(unit.applications || []).slice(0, 2).map(escape).join(" · ")}</p>
  </div>
</article>`;
}

function contributeBody() {
  return `<main class="contribute">
<p class="crumbs"><a href="index.html">Packages</a><span>/</span><strong>Contribute</strong></p>
<section class="contribute-head">
  <p class="eyebrow">Close the loop</p>
  <h1>Discover. Add.<br><em>Give one back.</em></h1>
  <p class="lede">loom is a catalog, not a black box. Every package has a source commit, tests, and a README. The same path that installs a package also feeds the next one.</p>
</section>
<section class="loop">
  <article><span>01</span><h2>Search</h2><p><code>loom search "load cell"</code> or use the catalog filters. Read the contract and the “when not to use” section.</p></article>
  <article><span>02</span><h2>Add</h2><p><code>loom add ads1115</code> copies the pinned unit and writes <code>loom.toml</code> and <code>loom.lock</code>.</p></article>
  <article><span>03</span><h2>Verify</h2><p><code>loom verify</code> checks installed hashes. <code>loom guard --staged</code> blocks an unprovenanced driver in CI.</p></article>
  <article><span>04</span><h2>Contribute</h2><p>Keep the contract, add host tests, and open a change against <a href="https://git.pcbsmt.me/rh/emb-loom">rh/emb-loom</a>. A new tag becomes a new catalog version.</p></article>
</section>
<section class="contribute-grid">
  <div>
    <h2>What a package contains</h2>
    <ul>
      <li><code>README.md</code> — purpose, parameters, memory, timing, integration, and when not to use.</li>
      <li><code>&lt;id&gt;.h</code> / <code>&lt;id&gt;.c</code> — C99, no allocation, no hidden board dependencies.</li>
      <li><code>tests/test_&lt;id&gt;.c</code> — host tests with a fake port.</li>
      <li><code>loom.json</code> entry — kind, tags, applications, compatibility.</li>
    </ul>
    <p>Keep SFRs, pins, clocks, and board wiring in a project adapter. The catalog unit is the reusable core.</p>
  </div>
  <div>
    <h2>Contribution path</h2>
    <ol>
      <li>Add the unit and its test under <code>drivers/</code> or <code>operators/</code>.</li>
      <li>Run <code>make test</code> and <code>node tools/loom/sync.mjs --check</code>.</li>
      <li>Open a pull request at <a href="https://git.pcbsmt.me/rh/emb-loom">git.pcbsmt.me/rh/emb-loom</a>.</li>
      <li>After review, tag a new version. The site is built from that tag.</li>
    </ol>
    <a class="button" href="https://git.pcbsmt.me/rh/emb-loom/issues/new">Request or propose a package ↗</a>
  </div>
</section>
<section class="contract-note">
  <h2>No silent forks</h2>
  <p>If a package is close but not exact, reuse the algorithm and document the difference in the project adapter. Do not edit a copied unit in place; record a local change in the project and keep the catalog contract intact.</p>
</section>
</main>`;
}

function unitBody(unit, lock) {
  return `<main class="detail">
<p class="crumbs"><a href="../index.html">Packages</a><span>/</span><a href="../index.html">${escape(unit.kind)}s</a><span>/</span><strong>${escape(unit.id)}</strong></p>
<section class="unit-head">
  <div>
    <p class="eyebrow">${escape(unit.kind)} package</p>
    <h1>${escape(unit.id)} <span class="version">v${escape(unit.version)}</span></h1>
    <p class="lede">${escape(unit.summary)}</p>
  </div>
  <div class="install-box">
    <span>Install</span>
    <code>loom add ${escape(unit.id)}</code>
    <button id="copy" type="button">Copy</button>
  </div>
</section>
<div class="unit-grid">
<article class="prose">${markdown(unit.readme)}</article>
<aside class="meta">
  <h2>Metadata</h2>
  <dl>
    <dt>Kind</dt><dd>${escape(unit.kind)}</dd>
    <dt>Version</dt><dd>${escape(unit.version)}</dd>
    <dt>Catalog</dt><dd>${escape(manifest.version)}</dd>
    <dt>Path</dt><dd><code>${escape(unit.path)}</code></dd>
    <dt>Commit</dt><dd><code>${escape((lock.commit || "").slice(0, 12))}</code></dd>
  </dl>
  <h2>Compatibility</h2>
  <div class="compat">${compat(unit.compatibility)}</div>
  <h2>Applications</h2>
  <ul class="apps">${(unit.applications || []).map(app => `<li>${escape(app)}</li>`).join("")}</ul>
  <h2>Provenance</h2>
  <p class="small">Pinned at <code>${escape(lock.ref || "")}</code>. Read the README and tests before adapting an adapter.</p>
  <a class="button" href="${escape(unit.source_url)}">View source at ${escape(lock.ref || "")} ↗</a>
</aside>
</div>
</main>
<script>
document.getElementById('copy').addEventListener('click', async event => {
  try { await navigator.clipboard.writeText('loom add ${escape(unit.id)}'); } catch {}
  event.target.textContent = 'Copied';
  setTimeout(() => event.target.textContent = 'Copy', 1200);
});
const globalSearch = document.getElementById('search');
globalSearch.addEventListener('keydown', event => {
  if (event.key === 'Enter' && globalSearch.value.trim()) {
    location.href = '../index.html?q=' + encodeURIComponent(globalSearch.value.trim());
  }
});
</script>`;
}

function compat(value = {}) {
  return [
    value.alloc ? "alloc" : "no alloc",
    value.float ? "float" : "no float",
    value.blocking ? "blocking" : "non-blocking",
  ].map(label => `<span>${escape(label)}</span>`).join("");
}

function markdown(text) {
  const lines = text.replace(/\r\n/g, "\n").split("\n");
  const out = [];
  let inCode = false;
  let code = [];
  let list = false;
  let skippedTitle = false;
  const closeList = () => { if (list) { out.push("</ul>"); list = false; } };
  for (const line of lines) {
    if (line.startsWith("```")) {
      if (inCode) { out.push(`<pre><code>${escape(code.join("\n"))}</code></pre>`); code = []; inCode = false; }
      else { closeList(); inCode = true; }
      continue;
    }
    if (inCode) { code.push(line); continue; }
    if (!line.trim()) { closeList(); continue; }
    if (line.startsWith("# ")) {
      closeList();
      if (!skippedTitle) { skippedTitle = true; continue; }
      out.push(`<h1>${inline(line.slice(2))}</h1>`);
      continue;
    }
    if (line.startsWith("## ")) { closeList(); out.push(`<h2>${inline(line.slice(3))}</h2>`); continue; }
    if (line.startsWith("### ")) { closeList(); out.push(`<h3>${inline(line.slice(4))}</h3>`); continue; }
    if (line.startsWith("- ")) {
      if (!list) { out.push("<ul>"); list = true; }
      out.push(`<li>${inline(line.slice(2))}</li>`);
      continue;
    }
    if (line.startsWith("> ")) { closeList(); out.push(`<blockquote>${inline(line.slice(2))}</blockquote>`); continue; }
    closeList();
    out.push(`<p>${inline(line)}</p>`);
  }
  if (inCode) out.push(`<pre><code>${escape(code.join("\n"))}</code></pre>`);
  closeList();
  return out.join("\n");
}

function inline(value) {
  return escape(value)
    .replace(/`([^`]+)`/g, "<code>$1</code>")
    .replace(/\*\*([^*]+)\*\*/g, "<strong>$1</strong>")
    .replace(/\[([^\]]+)\]\(([^)]+)\)/g, '<a href="$2">$1</a>');
}

function escape(value) {
  return String(value ?? "").replace(/[&<>"']/g, ch => ({
    "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;",
  }[ch]));
}

