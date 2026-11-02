#!/usr/bin/env node
// Validate the catalog shape before tests or a site build.

import fs from "node:fs";
import path from "node:path";

const root = path.resolve(process.argv[2] || process.cwd());
const manifest = JSON.parse(fs.readFileSync(path.join(root, "loom.json"), "utf8"));
const errors = [];
const ids = new Set();
const sections = [
  "Application scenarios",
  "Contract",
  "Parameters",
  "Memory and timing",
  "Integration",
  "When not to use",
  "Verification",
];

// A README opens with a purpose paragraph, then the contract sections. The
// closing sections are what a consumer reads before adopting a unit.
function checkReadme(unit, text) {
  if (!/^# \S/.test(text)) errors.push(`${unit.id}: README has no H1 title`);
  const head = text.split(/^## /m)[0].replace(/^# .*$/m, "").trim();
  if (!head) errors.push(`${unit.id}: README has no purpose paragraph before the first section`);
  const found = new Map();
  let current = null;
  for (const line of text.split("\n")) {
    const heading = /^## (.+?)\s*$/.exec(line);
    if (heading) {
      current = heading[1];
      found.set(current, []);
    } else if (current) {
      found.get(current).push(line);
    }
  }
  for (const name of sections) {
    if (!found.has(name)) errors.push(`${unit.id}: README missing section "${name}"`);
    else if (!found.get(name).join("\n").trim()) errors.push(`${unit.id}: README section "${name}" is empty`);
  }
  if (unit.kind === "driver") {
    if (!found.has("Adapters")) errors.push(`${unit.id}: driver README missing section "Adapters"`);
    else if (!found.get("Adapters").join("\n").trim()) errors.push(`${unit.id}: README section "Adapters" is empty`);
  }
}

for (const unit of manifest.units) {
  if (ids.has(unit.id)) errors.push(`duplicate id: ${unit.id}`);
  ids.add(unit.id);
  if (!/^[a-z][a-z0-9-]*$/.test(unit.id)) errors.push(`bad id: ${unit.id}`);
  if (unit.kind !== "driver" && unit.kind !== "operator") errors.push(`${unit.id}: bad kind`);
  if (!unit.version || !unit.summary) errors.push(`${unit.id}: missing version or summary`);
  if (!Array.isArray(unit.tags) || unit.tags.length === 0) errors.push(`${unit.id}: no tags`);
  if (!Array.isArray(unit.applications) || unit.applications.length === 0) errors.push(`${unit.id}: no applications`);
  if (!unit.compatibility || unit.compatibility.language !== "c99") errors.push(`${unit.id}: compatibility.language must be c99`);
  const directory = path.join(root, unit.path);
  if (!fs.existsSync(path.join(directory, "README.md"))) errors.push(`${unit.id}: missing README.md`);
  else checkReadme(unit, fs.readFileSync(path.join(directory, "README.md"), "utf8"));
  let files = [];
  try { files = fs.readdirSync(directory); } catch { errors.push(`${unit.id}: missing path ${unit.path}`); continue; }
  if (!files.some(file => file.endsWith(".h"))) errors.push(`${unit.id}: no header`);
  if (!files.some(file => file.endsWith(".c"))) errors.push(`${unit.id}: no source`);
  let tests = [];
  try { tests = fs.readdirSync(path.join(directory, "tests")); } catch { errors.push(`${unit.id}: missing tests/`); continue; }
  if (!tests.some(file => file.endsWith(".c"))) errors.push(`${unit.id}: no test source`);
}

if (errors.length) {
  console.error(`catalog check failed (${errors.length})`);
  for (const error of errors) console.error(`- ${error}`);
  process.exit(1);
}
console.log(`catalog check: ok (${manifest.units.length} units)`);
