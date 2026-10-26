import assert from "node:assert/strict";
import {execFileSync} from "node:child_process";
import fs from "node:fs";
import os from "node:os";
import path from "node:path";
import test from "node:test";

const root = path.resolve(path.dirname(new URL(import.meta.url).pathname), "../..");

test("new-unit scaffolds a catalog entry and Makefile wiring", () => {
  const temp = fs.mkdtempSync(path.join(os.tmpdir(), "loom-new-unit-"));
  fs.cpSync(path.join(root, "tools"), path.join(temp, "tools"), {recursive: true});
  fs.copyFileSync(path.join(root, "loom.json"), path.join(temp, "loom.json"));
  fs.copyFileSync(path.join(root, "Makefile"), path.join(temp, "Makefile"));
  execFileSync(process.execPath, [path.join(temp, "tools/loom/new-unit.mjs"), "demo-unit",
    "--kind", "driver", "--summary", "Demo unit"], {cwd: temp});
  for (const file of ["README.md", "demo-unit.h", "demo-unit.c", "tests/test_demo-unit.c"]) {
    assert.ok(fs.existsSync(path.join(temp, "drivers/demo-unit", file)), file);
  }
  const manifest = JSON.parse(fs.readFileSync(path.join(temp, "loom.json"), "utf8"));
  assert.ok(manifest.units.some(unit => unit.id === "demo-unit"));
  const makefile = fs.readFileSync(path.join(temp, "Makefile"), "utf8");
  assert.match(makefile, /\$\(BUILD\)\/test_demo-unit/);
  assert.doesNotThrow(() => execFileSync("make", ["-n", "build/test_demo-unit"], {cwd: temp, stdio: "pipe"}));
  fs.rmSync(temp, {recursive: true, force: true});
});
