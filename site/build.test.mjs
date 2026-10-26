import assert from "node:assert/strict";
import fs from "node:fs";
import os from "node:os";
import path from "node:path";
import {execFileSync} from "node:child_process";
import test from "node:test";

const root = path.resolve(path.dirname(new URL(import.meta.url).pathname), "..");
const out = fs.mkdtempSync(path.join(os.tmpdir(), "loom-site-"));
execFileSync(process.execPath, [path.join(root, "site/build.mjs"), "--out", out]);
const manifest = JSON.parse(fs.readFileSync(path.join(root, "loom.json"), "utf8"));

test("builds one page per unit plus the catalog", () => {
  assert.ok(fs.existsSync(path.join(out, "index.html")));
  assert.ok(fs.existsSync(path.join(out, "style.css")));
  for (const unit of manifest.units) {
    assert.ok(fs.existsSync(path.join(out, "units", `${unit.id}.html`)), unit.id);
  }
  const index = fs.readFileSync(path.join(out, "index.html"), "utf8");
  assert.match(index, /Drivers and algorithms/);
  assert.match(index, /data-search/);
  const detail = fs.readFileSync(path.join(out, "units", "median.html"), "utf8");
  assert.match(detail, /\.\.\/style\.css/);
  assert.match(detail, /\.\.\/index\.html/);
  assert.match(detail, /View source at v/);
});

test("writes a registry-shaped index", () => {
  const index = JSON.parse(fs.readFileSync(path.join(out, "index.json"), "utf8"));
  assert.equal(index.catalog, manifest.version);
  assert.equal(index.units.length, manifest.units.length);
  assert.ok(index.units.every(unit => unit.id && unit.summary));
});
