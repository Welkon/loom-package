#!/usr/bin/env node
// Build a static catalog from loom.json and each unit README.
// No runtime dependencies and no network access.

import {execFileSync} from "node:child_process";
import fs from "node:fs";
import crypto from "node:crypto";
import zlib from "node:zlib";
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
  : {ref: git(["describe", "--tags", "--abbrev=0"]) || `v${manifest.version}`, commit: git(["rev-parse", "HEAD"])};
const source = "https://git.pcbsmt.me/rh/emb-loom.git";
const logo = `<svg viewBox="0 0 32 32" width="22" height="22" aria-hidden="true"><path d="M16 3c-4.4 0-8 3.6-8 8v3H6v4h2v11h4V18h3v11h4V18h3v11h4V14c0-4.4-3.6-8-8-8h-2z" fill="currentColor"/><circle cx="16" cy="8" r="2.2" fill="var(--paper)"/></svg>`;
const favicon = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 32 32"><rect width="32" height="32" rx="7" fill="#1d1b17"/><path d="M16 5c-4 0-7 3-7 7v2H7v4h2v9h4v-9h2v9h4v-9h2v9h4v-9h2v-4h-2v-2c0-4-3-7-7-7h-2z" fill="#f4a261"/><circle cx="16" cy="10" r="1.8" fill="#1d1b17"/></svg>`;
const units = manifest.units.map(unit => {
  const directory = path.join(root, unit.path);
  const header = fs.readdirSync(directory).find(file => file.endsWith(".h"));
  const test = fs.readdirSync(path.join(directory, "tests")).find(file => file.endsWith(".c"));
  return {
    ...unit,
    readme: readReadme(unit.path),
    header,
    test,
    example: integrationExample(readReadme(unit.path)),
    api: header ? parseApi(path.join(directory, header)) : {functions: [], types: []},
    source_url: `${source.replace(/\.git$/, "")}/src/tag/${encodeURIComponent(lock.ref)}/${unit.path}`,
    header_url: header
      ? `${source.replace(/\.git$/, "")}/src/tag/${encodeURIComponent(lock.ref)}/${unit.path}/${header}`
      : `${source.replace(/\.git$/, "")}/src/tag/${encodeURIComponent(lock.ref)}/${unit.path}`,
    test_url: test
      ? `${source.replace(/\.git$/, "")}/src/tag/${encodeURIComponent(lock.ref)}/${unit.path}/tests/${test}`
      : `${source.replace(/\.git$/, "")}/src/tag/${encodeURIComponent(lock.ref)}/${unit.path}`,
  };
});

const registry = buildRegistry(manifest, units, new Set(manifest.yanked || []));
fs.rmSync(out, {recursive: true, force: true});
fs.mkdirSync(path.join(out, "units"), {recursive: true});
fs.mkdirSync(path.join(out, "tarballs"), {recursive: true});
fs.writeFileSync(path.join(out, "config.json"), JSON.stringify({
  dl: "tarballs/{id}-{version}.tar.gz",
  "auth-required": false,
}, null, 2) + "\n");
for (const [relative, content] of registry.index) {
  const file = path.join(out, relative);
  fs.mkdirSync(path.dirname(file), {recursive: true});
  fs.writeFileSync(file, content);
}
for (const [file, content] of registry.tarballs) {
  fs.writeFileSync(path.join(out, "tarballs", file), content);
}
fs.writeFileSync(path.join(out, "style.css"), fs.readFileSync(path.join(root, "site/style.css")));
fs.writeFileSync(path.join(out, "index.html"), page("Catalog", indexBody(units, lock), ""));
for (const unit of units) {
  fs.writeFileSync(path.join(out, "units", `${unit.id}.html`), page(unit.id, unitBody(unit, lock), "../"));
}
fs.writeFileSync(path.join(out, "contribute.html"), page("Contribute", contributeBody(), ""));
fs.writeFileSync(path.join(out, "docs.html"), page("Documentation", docsBody(units), ""));
fs.writeFileSync(path.join(out, "history.html"), page("History", historyBody(), ""));
fs.writeFileSync(path.join(out, "examples.html"), page("Examples", examplesBody(units), ""));
fs.writeFileSync(path.join(out, "request.html"), page("Request a package", requestBody(), ""));
fs.writeFileSync(path.join(out, "tests.html"), page("Compatibility", testsBody(units), ""));
fs.writeFileSync(path.join(out, "llms.txt"), llmsBody(units, lock, source));
fs.writeFileSync(path.join(out, "index.json"), JSON.stringify({
  schema: 1,
  catalog: manifest.version,
  source,
  ref: lock.ref,
  commit: lock.commit,
  units: units.map(unit => ({
    id: unit.id, kind: unit.kind, version: unit.version, summary: unit.summary,
    tags: unit.tags, applications: unit.applications, compatibility: unit.compatibility,
    requires: unit.requires || [], sha256: lock.units?.[unit.id]?.sha256 || "",
    tarball_sha256: registry.sha256.get(unit.id) || "",
    yanked: (manifest.yanked || []).includes(`${unit.id}@${unit.version}`),
    path: unit.path,
  })),
}, null, 2) + "\n");
console.log(`loom site: ${units.length} units -> ${out}`);

function indexPath(id) {
  if (id.length === 1) return `1/${id}`;
  if (id.length === 2) return `2/${id}`;
  if (id.length === 3) return `3/${id[0]}/${id}`;
  return `${id.slice(0, 2)}/${id.slice(2, 4)}/${id}`;
}

function buildRegistry(manifest, units, yanked) {
  const index = new Map();
  const tarballs = new Map();
  const sha256 = new Map();
  const license = git(["show", "HEAD:LICENSE"]);
  for (const unit of units) {
    const versions = new Map();
    for (const tag of git(["tag", "-l", `unit/${unit.id}/v*`]).split("\n").filter(Boolean)) {
      const info = readTagUnit(tag, unit.id);
      if (info) versions.set(info.version, info);
    }
    versions.set(unit.version, {
      id: unit.id, version: unit.version, kind: unit.kind, path: unit.path,
      requires: unit.requires || [], commit: lock.commit,
      files: readWorkingFiles(path.join(root, unit.path)), license,
    });
    const lines = [];
    for (const info of versions.values()) {
      if (!info.files.length) continue;
      const tarball = makeTarball(info, license);
      const sum = sha256Hex(tarball);
      const file = `${info.id}-${info.version}.tar.gz`;
      tarballs.set(file, tarball);
      sha256.set(`${info.id}@${info.version}`, sum);
      if (info.version === unit.version) sha256.set(unit.id, sum);
      lines.push(JSON.stringify({
        id: info.id, version: info.version, kind: info.kind, path: info.path,
        requires: info.requires, sha256: sum, commit: info.commit,
        yanked: yanked.has(`${info.id}@${info.version}`),
      }));
    }
    lines.sort();
    index.set(indexPath(unit.id), lines.join("\n") + "\n");
  }
  return {index, tarballs, sha256};
}

function readTagUnit(tag, id) {
  const raw = git(["show", `${tag}:loom.json`]);
  if (!raw) return null;
  let parsed;
  try { parsed = JSON.parse(raw); } catch { return null; }
  const unit = parsed.units?.find(item => item.id === id);
  if (!unit) return null;
  const pathName = unit.path;
  const tree = git(["ls-tree", "-r", "--name-only", tag, "--", pathName])
    .split("\n").filter(Boolean);
  const files = tree.map(file => ({
    relative: file.slice(pathName.length + 1),
    bytes: gitBuffer(["show", `${tag}:${file}`]),
  }));
  return {
    id, version: unit.version, kind: unit.kind, path: pathName,
    requires: unit.requires || [],
    commit: git(["rev-parse", `${tag}^{commit}`]),
    files,
    license: git(["show", `${tag}:LICENSE`]),
  };
}

function readWorkingFiles(directory) {
  const files = [];
  const walk = (current, relative) => {
    for (const entry of fs.readdirSync(current, {withFileTypes: true}).sort((a, b) => a.name.localeCompare(b.name))) {
      if (entry.name === "build" || entry.name === ".git" || entry.name.startsWith(".")) continue;
      const next = relative ? `${relative}/${entry.name}` : entry.name;
      if (entry.isDirectory()) walk(path.join(current, entry.name), next);
      else if (entry.isFile()) files.push({relative: next, bytes: fs.readFileSync(path.join(current, entry.name))});
    }
  };
  walk(directory, "");
  return files;
}

function makeTarball(info, license) {
  const files = new Map();
  for (const file of info.files) files.set(file.relative, file.bytes);
  if (license && !files.has("LICENSE")) files.set("LICENSE", Buffer.from(license));
  const rootName = `${info.id}-${info.version}`;
  const blocks = [];
  for (const [relative, bytes] of [...files].sort(([a], [b]) => a.localeCompare(b))) {
    blocks.push(tarHeader(`${rootName}/${relative}`, bytes.length), bytes);
    const pad = (512 - (bytes.length % 512)) % 512;
    if (pad) blocks.push(Buffer.alloc(pad));
  }
  blocks.push(Buffer.alloc(1024));
  return zlib.gzipSync(Buffer.concat(blocks), {level: 9});
}

function tarHeader(name, size) {
  if (Buffer.byteLength(name) > 100) throw new Error(`tar path too long: ${name}`);
  const block = Buffer.alloc(512);
  const text = (value, offset, length) => block.write(value, offset, length, "utf8");
  text(name, 0, 100);
  text("0000644\0", 100, 8);
  text("0000000\0", 108, 8);
  text("0000000\0", 116, 8);
  text(size.toString(8).padStart(11, "0") + "\0", 124, 12);
  text("00000000000\0", 136, 12);
  block.fill(0x20, 148, 156);
  text("0", 156, 1);
  text("ustar\0", 257, 6);
  text("00", 263, 2);
  block.fill(0x20, 265, 297);
  block.fill(0x20, 297, 329);
  let checksum = 0;
  for (const byte of block) checksum += byte;
  text(checksum.toString(8).padStart(6, "0") + "\0 ", 148, 8);
  return block;
}

function sha256Hex(bytes) {
  return crypto.createHash("sha256").update(bytes).digest("hex");
}

function gitBuffer(args) {
  try {
    return execFileSync("git", ["-C", root, ...args]);
  } catch {
    return Buffer.alloc(0);
  }
}

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
    <a href="${prefix}docs.html">Docs</a>
    <a href="${prefix}history.html">History</a>
    <a href="${prefix}contribute.html">Contribute</a>
    <a href="#about">About</a>
    <a href="${escape(source)}" class="source-link">Source ↗</a>
    <span class="ref">${escape(lock.ref || "")}</span>
  </nav>
</header>
${body}
<script>
const globalSearch = document.getElementById('search');
if (globalSearch) {
  globalSearch.addEventListener('keydown', event => {
    if (event.key === 'Enter' && globalSearch.value.trim()) {
      location.href = '${prefix}index.html?q=' + encodeURIComponent(globalSearch.value.trim());
    }
  });
}
</script>
<footer class="footer">
  <div class="footer-inner">
    <div><strong>loom</strong> <span>${escape(manifest.version)}</span></div>
    <div>${units.length} packages · pinned by commit · no install scripts</div>
    <div><a href="${escape(source)}">Repository</a> · <a href="${prefix}index.json">index.json</a> · <a href="${prefix}docs.html">Docs</a> · <a href="${prefix}examples.html">Examples</a> · <a href="${prefix}tests.html">Compatibility</a> · <a href="${prefix}history.html">History</a> · <a href="${prefix}llms.txt">llms.txt</a> · <a href="${escape(source.replace(/\.git$/, ""))}/raw/branch/main/AGENTS.md">AGENTS.md</a></div>
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

function llmsBody(units, lock, source) {
  const repo = source.replace(/\.git$/, "");
  return `# loom\n\n> Reusable C drivers and algorithms for embedded systems. Every unit is pinned by source commit and SHA-256.\n\n## For AI agents\n\n- Read ${repo}/raw/branch/main/AGENTS.md before changing the catalog.\n- Search \`loom.json\` and the unit folders before writing. Reuse or extend an adapter; do not create a near-duplicate.\n- Scaffold with \`node tools/loom/new-unit.mjs <id> --kind driver|operator\`.\n- Do not copy GPL or vendor source. Cite the real behavior reference and license.\n- Run \`make test\` and \`node tools/loom/check-catalog.mjs\`. Report unverified bench behavior.\n- Do not tag, bump the catalog version, or commit generated \`site/dist\`.\n\n## Catalog\n\n${units.map(unit => `- [${unit.id}](https://loom.pcbsmt.me/units/${unit.id}.html) — ${unit.summary} (${unit.kind}, ${unit.version})`).join("\n")}\n\n## Registry\n\n- [index.json](https://loom.pcbsmt.me/index.json)\n- [Documentation](https://loom.pcbsmt.me/docs.html)\n- [Examples](https://loom.pcbsmt.me/examples.html)\n- [Compatibility](https://loom.pcbsmt.me/tests.html)\n- [Request a package](https://loom.pcbsmt.me/request.html)\n- [Source](${repo})\n- [Catalog ref](${repo}/src/tag/${encodeURIComponent(lock.ref)})\n`;
}

function integrationExample(readme) {
  const marker = readme.indexOf("## Integration");
  if (marker < 0) return "";
  const rest = readme.slice(marker);
  const match = /```(?:c)?\n([\s\S]*?)```/.exec(rest);
  return match ? match[1].trim() : "";
}

function docsTabs(active) {
  const tabs = [["docs.html", "Documentation"], ["examples.html", "Examples"], ["tests.html", "Compatibility"]];
  return `<nav class="docs-tabs">${tabs.map(([href, label]) => `<a href="${href}"${href === active ? ' class="is-active"' : ""}>${label}</a>`).join("")}</nav>`;
}

function requestBody() {
  return `<main class="docs request">
<p class="crumbs"><a href="index.html">Packages</a><span>/</span><a href="contribute.html">Contribute</a><span>/</span><strong>Request</strong></p>
<section class="docs-head">
  <p class="eyebrow">Package request</p>
  <h1>Ask for a<br><em>missing unit.</em></h1>
  <p class="lede">Describe the device and the gap. The form opens a pre-filled Forgejo issue; you can also copy the text and send it another way.</p>
</section>
<form id="request-form" class="request-form">
  <label>Package id <input name="id" placeholder="example-driver" required></label>
  <label>Kind <select name="kind"><option>driver</option><option>operator</option></select></label>
  <label>Device or source <input name="source" placeholder="part number, datasheet, or real project"></label>
  <label>Use case <textarea name="use" rows="4" placeholder="What should a project be able to do?"></textarea></label>
  <label>Why existing units do not fit <textarea name="gap" rows="3"></textarea></label>
  <div class="form-actions"><button type="submit">Open Forgejo issue ↗</button><button type="button" id="copy-request">Copy text</button></div>
</form>
<pre id="request-preview" class="request-preview" aria-live="polite"></pre>
<p class="small">Submitting an issue requires a Forgejo account at <a href="https://git.pcbsmt.me/user/login">git.pcbsmt.me</a>. Registration is currently disabled; ask the maintainer for an invite if you do not have one.</p>
</main>
<script>
const form = document.getElementById('request-form');
const preview = document.getElementById('request-preview');
function text() {
  const data = new FormData(form);
  return [
    '**Package id**', data.get('id') || '', '',
    '**Kind**', data.get('kind') || '', '',
    '**Device or source**', data.get('source') || '', '',
    '**Use case**', data.get('use') || '', '',
    '**Why existing units do not fit**', data.get('gap') || '',
  ].join('\n');
}
function update() { preview.textContent = text(); }
form.addEventListener('input', update);
form.addEventListener('submit', event => {
  event.preventDefault();
  const data = new FormData(form);
  const title = '[package] ' + (data.get('id') || 'request');
  const url = 'https://git.pcbsmt.me/rh/emb-loom/issues/new?template=package-request.md&title=' +
    encodeURIComponent(title) + '&body=' + encodeURIComponent(text());
  window.location.href = url;
});
document.getElementById('copy-request').addEventListener('click', async () => {
  try { await navigator.clipboard.writeText(text()); } catch {}
});
update();
</script>`;
}

function examplesBody(units) {
  const group = kind => units.filter(unit => unit.kind === kind).map(unit => `<article class="example-card">
    <header><a class="example-name" href="units/${escape(unit.id)}.html">${escape(unit.id)}</a><span class="kind ${escape(unit.kind)}">${escape(unit.kind)}</span></header>
    <p>${escape(unit.summary)}</p>
    ${unit.example ? `<pre><code>${escape(unit.example)}</code></pre>` : `<p class="small">No integration example.</p>`}
    <a class="example-link" href="units/${escape(unit.id)}.html#quickstart">Full contract and API →</a>
  </article>`).join("\n");
  return `<main class="docs">
<p class="crumbs"><a href="index.html">Packages</a><span>/</span><strong>Examples</strong></p>
${docsTabs("examples.html")}
<section class="docs-head"><p class="eyebrow">Integration examples</p><h1>One file,<br><em>one adapter.</em></h1><p class="lede">Each example is the integration block from the package README. Board wiring stays in the project adapter; the catalog unit stays portable.</p></section>
<section class="example-section"><h2>Operators <span>${units.filter(unit => unit.kind === "operator").length}</span></h2><div class="example-grid">${group("operator")}</div></section>
<section class="example-section"><h2>Drivers <span>${units.filter(unit => unit.kind === "driver").length}</span></h2><div class="example-grid">${group("driver")}</div></section>
</main>`;
}

function testsBody(units) {
  const rows = units.map(unit => {
    const compatibility = unit.compatibility || {};
    return `<tr>
      <td><a href="units/${escape(unit.id)}.html">${escape(unit.id)}</a></td>
      <td><span class="kind ${escape(unit.kind)}">${escape(unit.kind)}</span></td>
      <td>${compatibility.alloc ? "alloc" : "—"}</td>
      <td>${compatibility.float ? "float" : "—"}</td>
      <td>${compatibility.blocking ? "blocking" : "—"}</td>
      <td><a href="${escape(unit.test_url)}">test ↗</a></td>
    </tr>`;
  }).join("\n");
  return `<main class="docs">
<p class="crumbs"><a href="index.html">Packages</a><span>/</span><strong>Compatibility</strong></p>
${docsTabs("tests.html")}
<section class="docs-head"><p class="eyebrow">Compatibility matrix</p><h1>Pick by<br><em>constraint.</em></h1><p class="lede">Every package is C99 and has a deterministic host test. The matrix shows the three compatibility constraints that change how a unit fits a target.</p></section>
<section class="matrix-wrap">
<table class="matrix"><thead><tr><th>Package</th><th>Kind</th><th>Alloc</th><th>Float</th><th>Blocking</th><th>Test</th></tr></thead><tbody>${rows}</tbody></table>
</section>
</main>`;
}

function historyBody() {
  const changelog = fs.existsSync(path.join(root, "CHANGELOG.md"))
    ? fs.readFileSync(path.join(root, "CHANGELOG.md"), "utf8")
    : "";
  return `<main class="history">
<p class="crumbs"><a href="index.html">Packages</a><span>/</span><strong>History</strong></p>
<section class="docs-head">
  <p class="eyebrow">Catalog history</p>
  <h1>What changed,<br><em>release by release.</em></h1>
  <p class="lede">The catalog is versioned by tag. Each release records the units added and the source references behind them.</p>
</section>
<article class="prose history-prose">${markdown(changelog)}</article>
</main>`;
}

function docsBody(units) {
  const group = kind => units
    .filter(unit => unit.kind === kind)
    .map(unit => `<div class="doc-row">
      <a class="doc-name" href="units/${escape(unit.id)}.html">${escape(unit.id)}</a>
      <span class="doc-summary">${escape(unit.summary)}</span>
      <span class="doc-links"><a href="units/${escape(unit.id)}.html">Overview</a><span>·</span><a href="units/${escape(unit.id)}.html#api">API</a></span>
    </div>`)
    .join("\n");
  return `<main class="docs">
<p class="crumbs"><a href="index.html">Packages</a><span>/</span><strong>Documentation</strong></p>
${docsTabs("docs.html")}
<section class="docs-head">
  <p class="eyebrow">Reference documentation</p>
  <h1>Every package,<br><em>its contract.</em></h1>
  <p class="lede">Each page has the README overview and an API section extracted from the public header. The source link points at the same tagged revision the catalog uses.</p>
</section>
<section class="doc-section"><h2>Operators <span>${units.filter(unit => unit.kind === "operator").length}</span></h2><div class="doc-list">${group("operator")}</div></section>
<section class="doc-section"><h2>Drivers <span>${units.filter(unit => unit.kind === "driver").length}</span></h2><div class="doc-list">${group("driver")}</div></section>
</main>`;
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
  <article><span>04</span><h2>Contribute</h2><p><code>node tools/loom/new-unit.mjs &lt;id&gt;</code> scaffolds the contract and test. Run <code>make test</code>, then open a change against <a href="https://git.pcbsmt.me/rh/emb-loom">rh/emb-loom</a>. A new tag becomes a new catalog version.</p></article>
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
      <li>Run <code>node tools/loom/new-unit.mjs &lt;id&gt; --kind driver|operator</code>.</li>
      <li>Fill the README contract and behavior tests; run <code>make test</code> and <code>make check-catalog</code>.</li>
      <li>Open a pull request at <a href="https://git.pcbsmt.me/rh/emb-loom">git.pcbsmt.me/rh/emb-loom</a>.</li>
      <li>After review, tag a new version. The site is built from that tag.</li>
    </ol>
    <a class="button" href="request.html">Request or propose a package →</a>
  </div>
</section>
<section class="contract-note">
  <h2>No silent forks</h2>
  <p>If a package is close but not exact, reuse the algorithm and document the difference in the project adapter. Do not edit a copied unit in place; record a local change in the project and keep the catalog contract intact.</p>
</section>
<section class="ai-guide" id="ai">
  <p class="eyebrow">For AI agents</p>
  <h2>Read the contract before you generate.</h2>
  <p>Agents are welcome to contribute, but the catalog is not a prompt target. Search first, name the real source, write the host tests, and report what is still unverified. Do not copy vendor code or invent a register value.</p>
  <pre><code>Read AGENTS.md. Follow the loom contribution rules.
Search loom.json before writing. Use tools/loom/new-unit.mjs.
Do not copy GPL or vendor source. Run make test and check-catalog.
Report the source, license, tests, and any bench verification still missing.</code></pre>
  <p><a href="${escape(source.replace(/\.git$/, ""))}/raw/branch/main/AGENTS.md">Read AGENTS.md</a> · <a href="llms.txt">Machine-readable llms.txt</a> · <a href="${escape(source.replace(/\.git$/, ""))}/issues/new">Propose a package</a></p>
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
<nav class="unit-toc"><a href="#overview">Overview</a><a href="#quickstart">Quick start</a><a href="#api">API</a><a href="${escape(unit.header_url)}">Header ↗</a><a href="${escape(unit.test_url)}">Tests ↗</a><a href="${escape(unit.source_url)}">Source ↗</a></nav>
${unit.example ? `<section id="quickstart" class="quickstart"><div><p class="eyebrow">Quick start</p><h2>Use it in a project</h2></div><pre><code>${escape(unit.example)}</code></pre></section>` : ""}
<div class="unit-grid">
<article class="prose" id="overview">${markdown(unit.readme)}${apiBody(unit)}</article>
<aside class="meta">
  <h2>Metadata</h2>
  <dl>
    <dt>Kind</dt><dd>${escape(unit.kind)}</dd>
    <dt>Version</dt><dd>${escape(unit.version)}</dd>
    <dt>Catalog</dt><dd>${escape(manifest.version)}</dd>
    <dt>Path</dt><dd><code>${escape(unit.path)}</code></dd>
    <dt>Header</dt><dd><a href="${escape(unit.header_url)}">${escape(unit.header)} ↗</a></dd>
    <dt>Tests</dt><dd><a href="${escape(unit.test_url)}">host test ↗</a></dd>
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
</script>`;
}

function parseApi(file) {
  const text = fs.readFileSync(file, "utf8");
  const types = [];
  const typePattern = /typedef\s+(struct|enum)\s*(?:\w+\s*)?\{([\s\S]*?)\}\s*(\w+)\s*;/g;
  let match;
  while ((match = typePattern.exec(text)) !== null) {
    const fields = match[2].split("\n").map(line => line.trim()).filter(Boolean);
    types.push({kind: match[1], name: match[3], fields});
  }
  const withoutTypes = text.replace(typePattern, "");
  const functions = [];
  const functionPattern = /(^|\n)\s*([A-Za-z_][\w\s*]+?)\s+([A-Za-z_]\w*)\s*\(([^;{}]*)\)\s*;/g;
  while ((match = functionPattern.exec(withoutTypes)) !== null) {
    if (match[2].includes("static") || match[2].includes("typedef")) continue;
    functions.push(`${match[2].trim()} ${match[3]}(${match[4].replace(/\s+/g, " ").trim()});`);
  }
  return {functions, types};
}

function apiBody(unit) {
  const functions = unit.api.functions.length
    ? unit.api.functions.map(signature => `<div class="api-row"><code>${escape(signature)}</code></div>`).join("")
    : `<p class="small">No public functions in this header.</p>`;
  const types = unit.api.types.length
    ? unit.api.types.map(type => `<div class="api-type"><code>${escape(type.name)}</code> <span>${escape(type.kind)}</span><pre><code>${escape(type.fields.join("\n"))}</code></pre></div>`).join("")
    : `<p class="small">No public types in this header.</p>`;
  return `<section id="api" class="api">
  <div class="section-head"><h2>API</h2><a href="${escape(unit.header_url)}">${escape(unit.header)} ↗</a></div>
  <p class="small">Extracted from the public header at the catalog tag. The README contract above is the behavior reference.</p>
  <h3>Functions</h3>
  <div class="api-list">${functions}</div>
  <h3>Types</h3>
  <div class="api-types">${types}</div>
</section>`;
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

