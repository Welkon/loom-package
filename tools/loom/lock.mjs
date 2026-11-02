#!/usr/bin/env node
// Generate or check the catalog lock from the current unit files.
import crypto from "node:crypto";
import fs from "node:fs";
import path from "node:path";
import {execFileSync} from "node:child_process";

const rootArg = process.argv.slice(2).find(value => !value.startsWith("--"));
const root = path.resolve(rootArg || ".");
const check = process.argv.includes("--check");
const manifest = JSON.parse(fs.readFileSync(path.join(root, "loom.json"), "utf8"));
const refArg = process.argv.find(value => value.startsWith("--ref="));
const ref = refArg ? refArg.slice("--ref=".length) : `v${manifest.version}`;
const git = args => execFileSync("git", ["-C", root, ...args], {
  encoding: "utf8",
  stdio: ["ignore", "pipe", "pipe"],
}).trim();
let commit = "";
try { commit = git(["rev-parse", `${ref}^{commit}`]); } catch { commit = git(["rev-parse", "HEAD"]); }

function hashDirectory(directory) {
  const files = [];
  const walk = relative => {
    for (const entry of fs.readdirSync(path.join(directory, relative), {withFileTypes: true})) {
      if (entry.name === ".git" || entry.name === "build") continue;
      const name = relative ? `${relative}/${entry.name}` : entry.name;
      if (entry.isDirectory()) walk(name);
      else if (entry.isFile()) files.push(name);
    }
  };
  walk("");
  files.sort();
  const hash = crypto.createHash("sha256");
  for (const file of files) {
    hash.update(file);
    hash.update(Buffer.from([0]));
    hash.update(fs.readFileSync(path.join(directory, file)));
    hash.update(Buffer.from([0]));
  }
  return hash.digest("hex");
}

const units = {};
for (const unit of [...manifest.units].sort((a, b) => a.id.localeCompare(b.id))) {
  units[unit.id] = {version: unit.version, sha256: hashDirectory(path.join(root, unit.path))};
}

// Hash a unit directory as it is stored in a commit, so the lock can prove that
// the published pin still matches the tagged content.
function hashCommitDirectory(commit, directory) {
  const prefix = directory.split(path.sep).join("/");
  const listing = git(["ls-tree", "-r", "--name-only", commit, "--", prefix]);
  const files = listing.split("\n").filter(Boolean)
    .map(name => name.slice(prefix.length + 1))
    .filter(name => name && !name.split("/").some(part => part === "build" || part === ".git"))
    .sort();
  const hash = crypto.createHash("sha256");
  for (const name of files) {
    const blob = execFileSync("git", ["-C", root, "cat-file", "blob", `${commit}:${prefix}/${name}`], {
      stdio: ["ignore", "pipe", "pipe"],
    });
    hash.update(name);
    hash.update(Buffer.from([0]));
    hash.update(blob);
    hash.update(Buffer.from([0]));
  }
  return hash.digest("hex");
}
const lock = {
  schema: 1,
  source: "https://github.com/Welkon/loom-package.git",
  ref,
  commit,
  units,
};
const text = JSON.stringify(lock, null, 2) + "\n";
const file = path.join(root, "loom.lock");
if (check) {
  let refExists = true;
  try { git(["rev-parse", "--verify", `refs/tags/${ref}`]); } catch { refExists = false; }
  if (!refExists) {
    console.error(`loom.lock: catalog ref ${ref} does not exist; tag the release before checking the lock`);
    process.exit(1);
  }
  const drifted = manifest.units
    .filter(unit => hashCommitDirectory(commit, unit.path) !== units[unit.id].sha256)
    .map(unit => unit.id);
  if (drifted.length) {
    console.error(`loom.lock: ${drifted.length} unit(s) differ from ${ref} (${commit.slice(0, 7)}); bump the unit version and re-tag`);
    for (const id of drifted) console.error(`- ${id}`);
    process.exit(1);
  }
  const actual = fs.existsSync(file) ? fs.readFileSync(file, "utf8") : "";
  if (actual !== text) {
    console.error("loom.lock is stale; run node tools/loom/lock.mjs");
    process.exit(1);
  }
  console.log(`loom.lock: ok (${manifest.units.length} units, ${ref}, ${commit.slice(0, 7)})`);
} else {
  fs.writeFileSync(file, text);
  console.log(`loom.lock: wrote ${manifest.units.length} units at ${ref}`);
}
