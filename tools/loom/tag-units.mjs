#!/usr/bin/env node
// Check or create the immutable per-unit version tags.
import fs from "node:fs";
import path from "node:path";
import {execFileSync} from "node:child_process";

const rootArg = process.argv.slice(2).find(value => !value.startsWith("--"));
const root = path.resolve(rootArg || ".");
const check = process.argv.includes("--check");
const manifest = JSON.parse(fs.readFileSync(path.join(root, "loom.json"), "utf8"));
const git = args => execFileSync("git", ["-C", root, ...args], {encoding: "utf8"}).trim();
const missing = [];

for (const unit of manifest.units) {
  const tag = `unit/${unit.id}/v${unit.version}`;
  let exists = true;
  try { git(["rev-parse", "--verify", `refs/tags/${tag}`]); }
  catch { exists = false; }
  if (!exists) {
    missing.push(tag);
    if (!check) {
      git(["tag", "-a", tag, "-m", `${unit.id} ${unit.version}`]);
    }
  }
}

if (missing.length) {
  if (check) {
    console.error(`missing unit tags (${missing.length}):`);
    for (const tag of missing) console.error(`- ${tag}`);
    process.exit(1);
  }
  console.log(`created ${missing.length} unit tags; push them with git push --tags`);
} else {
  console.log(`unit tags: ok (${manifest.units.length})`);
}
