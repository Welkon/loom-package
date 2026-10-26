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
  : {ref: git(["describe", "--tags", "--always"]), commit: git(["rev-parse", "HEAD"])};
const source = "https://git.pcbsmt.me/rh/emb-loom.git";
const units = manifest.units.map(unit => ({
  ...unit,
  readme: readReadme(unit.path),
  source_url: `${source.replace(/\.git$/, "")}/src/tag/${encodeURIComponent(lock.ref)}/${unit.path}`,
}));

fs.rmSync(out, {recursive: true, force: true});
fs.mkdirSync(path.join(out, "units"), {recursive: true});
fs.writeFileSync(path.join(out, "style.css"), fs.readFileSync(path.join(root, "site/style.css")));
fs.writeFileSync(path.join(out, "index.html"), page("Catalog", indexBody(units, manifest, lock)));
for (const unit of units) {
  fs.writeFileSync(path.join(out, "units", `${unit.id}.html`), page(unit.id, unitBody(unit, lock, source), "../"));
}
fs.writeFileSync(path.join(out, "index.json"), JSON.stringify({
  schema: 1,
  catalog: manifest.version,
  source,
  ref: lock.ref,
  commit: lock.commit,
  units: units.map(unit => ({
    id: unit.id,
    kind: unit.kind,
    version: unit.version,
    summary: unit.summary,
    tags: unit.tags,
    applications: unit.applications,
    compatibility: unit.compatibility,
    path: unit.path,
  })),
}, null, 2) + "\n");
console.log(`loom site: ${units.length} units -> ${out}`);

function git(args) {
  try {
    return execFileSync("git", ["-C", root, ...args], {encoding: "utf8"}).trim();
  } catch {
    return "";
  }
}

function readReadme(relative) {
  const file = path.join(root, relative, "README.md");
  return fs.existsSync(file) ? fs.readFileSync(file, "utf8") : "";
}

function page(title, body, prefix = "") {
  return `<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>${escape(title)} · loom</title>
<meta name="description" content="Pinned embedded drivers and algorithms, with provenance.">
<link rel="stylesheet" href="${prefix}style.css">
</head>
<body>
<header class="top">
  <a class="brand" href="${prefix}index.html"><span class="mark">◈</span> loom</a>
  <nav>
    <a href="${prefix}index.html">Catalog</a>
    <a href="${escape(source)}">Source</a>
    <span class="ref">${escape(lock.ref || "")}</span>
  </nav>
</header>
${body}
<footer class="foot">
  <span>loom ${escape(manifest.version)}</span>
  <span>${units.length} units · pinned by commit · no install scripts</span>
</footer>
</body>
</html>`;
}

function indexBody(units, manifest, lock) {
  const data = JSON.stringify(units.map(unit => ({
    id: unit.id, kind: unit.kind, summary: unit.summary, tags: unit.tags,
    applications: unit.applications, compatibility: unit.compatibility,
  }))).replace(/</g, "\\u003c");
  return `<main>
<section class="hero">
  <p class="eyebrow">Pinned catalog · ${escape(lock.ref || "")}</p>
  <h1>Reusable drivers<br>and algorithms.</h1>
  <p class="lede">Small, tested C units for embedded work. Every unit is pinned by source commit and SHA-256, and installed with <code>loom add</code>.</p>
  <div class="searchbar">
    <input id="search" type="search" placeholder="Search id, tag, application…" autocomplete="off" aria-label="Search catalog">
    <select id="kind" aria-label="Filter by kind">
      <option value="">All kinds</option>
      <option value="operator">Operators</option>
      <option value="driver">Drivers</option>
    </select>
    <span id="count" class="count"></span>
  </div>
</section>
<section class="grid" id="grid">
${units.map(card).join("\n")}
</section>
<p class="empty" id="empty" hidden>No unit matches that filter.</p>
</main>
<script id="catalog" type="application/json">${data}</script>
<script>
const cards = [...document.querySelectorAll('.card')];
const search = document.getElementById('search');
const kind = document.getElementById('kind');
const count = document.getElementById('count');
const empty = document.getElementById('empty');
function apply() {
  const q = search.value.trim().toLowerCase();
  let shown = 0;
  for (const card of cards) {
    const hay = card.dataset.search;
    const ok = (!q || hay.includes(q)) && (!kind.value || card.dataset.kind === kind.value);
    card.hidden = !ok;
    if (ok) shown++;
  }
  count.textContent = shown + ' / ' + cards.length;
  empty.hidden = shown !== 0;
}
search.addEventListener('input', apply);
kind.addEventListener('change', apply);
apply();
</script>`;
}

function card(unit) {
  const hay = [unit.id, unit.summary, ...(unit.tags || []), ...(unit.applications || [])].join(" ").toLowerCase();
  return `<article class="card" data-kind="${escape(unit.kind)}" data-search="${escape(hay)}">
  <div class="card-top"><span class="kind ${escape(unit.kind)}">${escape(unit.kind)}</span><span class="ver">${escape(unit.version)}</span></div>
  <h2><a href="units/${escape(unit.id)}.html">${escape(unit.id)}</a></h2>
  <p>${escape(unit.summary)}</p>
  <div class="tags">${(unit.tags || []).map(tag => `<span>${escape(tag)}</span>`).join("")}</div>
  <ul class="apps">${(unit.applications || []).slice(0, 3).map(app => `<li>${escape(app)}</li>`).join("")}</ul>
  <div class="compat">${compat(unit.compatibility)}</div>
</article>`;
}

function unitBody(unit, lock, source) {
  const repo = source.replace(/\.git$/, "");
  return `<main class="detail">
<p class="crumbs"><a href="../index.html">Catalog</a> / ${escape(unit.kind)} / ${escape(unit.id)}</p>
<section class="unit-head">
  <div>
    <p class="eyebrow">${escape(unit.kind)} · ${escape(unit.version)}</p>
    <h1>${escape(unit.id)}</h1>
    <p class="lede">${escape(unit.summary)}</p>
  </div>
  <code class="install">loom add ${escape(unit.id)}</code>
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
  <p class="small">Pinned at <code>${escape(lock.ref || "")}</code>. Read the unit README and tests before adapting an adapter.</p>
  <a class="button" href="${escape(unit.source_url)}">View source at ${escape(lock.ref || "")}</a>
</aside>
</div>
</main>`;
}

function compat(value = {}) {
  const labels = [
    value.alloc ? "alloc" : "no alloc",
    value.float ? "float" : "no float",
    value.blocking ? "blocking" : "non-blocking",
  ];
  return labels.map(label => `<span>${escape(label)}</span>`).join("");
}

function markdown(text) {
  const lines = text.replace(/\r\n/g, "\n").split("\n");
  const out = [];
  let inCode = false;
  let code = [];
  let list = false;
  const closeList = () => { if (list) { out.push("</ul>"); list = false; } };
  for (const line of lines) {
    if (line.startsWith("```")) {
      if (inCode) { out.push(`<pre><code>${escape(code.join("\n"))}</code></pre>`); code = []; inCode = false; }
      else { closeList(); inCode = true; }
      continue;
    }
    if (inCode) { code.push(line); continue; }
    if (!line.trim()) { closeList(); continue; }
    if (line.startsWith("# ")) { closeList(); out.push(`<h1>${inline(line.slice(2))}</h1>`); continue; }
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
