#!/usr/bin/env node
// Mark a unit version as yanked. The version and its tarball stay in history.
import fs from "node:fs";
import path from "node:path";
import {execFileSync} from "node:child_process";

const args = process.argv.slice(2);
const check = args.includes("--check");
const undo = args.includes("--undo");
const root = path.resolve(args.find(value => !value.startsWith("--") && !value.includes("@")) || ".");
const spec = args.find(value => !value.startsWith("--") && value.includes("@"));
const manifestPath = path.join(root, "loom.json");
const manifest = JSON.parse(fs.readFileSync(manifestPath, "utf8"));
const yanked = new Set(manifest.yanked || []);
const git = (...parts) => {
  try { return execFileSync("git", ["-C", root, ...parts], {encoding: "utf8"}).trim(); }
  catch { return ""; }
};

if (check) {
  const known = new Set();
  for (const unit of manifest.units) {
    known.add(`${unit.id}@${unit.version}`);
    for (const tag of git("tag", "-l", `unit/${unit.id}/v*`).split("\n")) {
      if (tag) known.add(tag.replace(/^unit\/(.*)\/v/, "$1@"));
    }
  }
  const errors = [];
  for (const value of yanked) {
    if (!known.has(value)) errors.push(`unknown yanked version: ${value}`);
  }
  if (errors.length) {
    console.error(`yank check failed (${errors.length})`);
    for (const error of errors) console.error(`- ${error}`);
    process.exit(1);
  }
  console.log(`yank check: ok (${yanked.size} yanked)`);
  process.exit(0);
}

if (!spec || !spec.includes("@")) {
  console.error("usage: yank.mjs ID@VERSION [--undo] [--check]");
  process.exit(2);
}
const [id, version] = spec.split("@");
const unit = manifest.units.find(item => item.id === id);
if (!unit) {
  console.error(`unknown unit: ${id}`);
  process.exit(2);
}
const tagged = Boolean(git("rev-parse", "--verify", `refs/tags/unit/${id}/v${version}`));
if (unit.version !== version && !tagged) {
  console.error(`unknown version: ${id}@${version}`);
  process.exit(2);
}
if (undo) yanked.delete(spec);
else yanked.add(spec);
manifest.yanked = [...yanked].sort();
fs.writeFileSync(manifestPath, JSON.stringify(manifest, null, 2) + "\n");
console.log(`${undo ? "unyanked" : "yanked"} ${spec}`);
