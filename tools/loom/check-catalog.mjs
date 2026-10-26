#!/usr/bin/env node
// Validate the catalog shape before tests or a site build.

import fs from "node:fs";
import path from "node:path";

const root = path.resolve(process.argv[2] || process.cwd());
const manifest = JSON.parse(fs.readFileSync(path.join(root, "loom.json"), "utf8"));
const errors = [];
const ids = new Set();

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
