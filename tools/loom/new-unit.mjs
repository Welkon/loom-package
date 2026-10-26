#!/usr/bin/env node
// Scaffold a catalog unit and wire its host test into the Makefile.
// Usage: node tools/loom/new-unit.mjs <id> --kind driver|operator [--summary TEXT]

import fs from "node:fs";
import path from "node:path";

const args = process.argv.slice(2);
const id = args.find(value => !value.startsWith("--"));
const option = name => {
  const index = args.indexOf(`--${name}`);
  return index >= 0 ? args[index + 1] : undefined;
};
const kind = option("kind") || "driver";
const summary = option("summary") || `Describe ${id}.`;
const root = path.resolve(option("root") || process.cwd());

if (!id || !/^[a-z][a-z0-9-]*$/.test(id)) {
  console.error("usage: node tools/loom/new-unit.mjs <id> --kind driver|operator");
  process.exit(2);
}
if (kind !== "driver" && kind !== "operator") {
  console.error("kind must be driver or operator");
  process.exit(2);
}
const manifestPath = path.join(root, "loom.json");
const makefilePath = path.join(root, "Makefile");
if (!fs.existsSync(manifestPath) || !fs.existsSync(makefilePath)) {
  console.error("run this from the emb-loom catalog root");
  process.exit(1);
}
const manifest = JSON.parse(fs.readFileSync(manifestPath, "utf8"));
if (manifest.units.some(unit => unit.id === id)) {
  console.error(`unit already exists: ${id}`);
  process.exit(1);
}
const directory = `${kind === "driver" ? "drivers" : "operators"}/${id}`;
const target = path.join(root, directory);
if (fs.existsSync(target)) {
  console.error(`directory already exists: ${directory}`);
  process.exit(1);
}

const guard = id.toUpperCase().replaceAll("-", "_");
const files = {
  "README.md": `# ${id}\n\n${summary}\n\n## Application scenarios\n\n- Add a concrete use case.\n- Add a second use case.\n\n## Contract\n\n\`\`\`c\n#include "${id}.h"\n\`\`\`\n\n## Parameters\n\n- Document every configuration value and its range.\n\n## Memory and timing\n\n- State the static memory and the worst-case cost.\n\n## Integration\n\n\`\`\`c\n${guard.toLowerCase()}_port_t port = board_port();\n${id.replaceAll("-", "_")}_init(&port);\n\`\`\`\n\n## Adapters\n\n- Keep SFRs, pins, clocks, and board wiring in the project adapter.\n\n## When not to use\n\n- State the boundary.\n\n## Verification\n\n\`make test\` covers the scaffold path. Replace this with behavior checks.\n`,
  [`${id}.h`]: `#ifndef ${guard}_H\n#define ${guard}_H\n\n#include <stdint.h>\n\n#ifdef __cplusplus\nextern "C" {\n#endif\n\ntypedef struct {\n    void *context;\n} ${id.replaceAll("-", "_")}_port_t;\n\nvoid ${id.replaceAll("-", "_")}_init(${id.replaceAll("-", "_")}_port_t *port);\n\n#ifdef __cplusplus\n}\n#endif\n\n#endif\n`,
  [`${id}.c`]: `#include "${id}.h"\n\nvoid ${id.replaceAll("-", "_")}_init(${id.replaceAll("-", "_")}_port_t *port)\n{\n    (void)port;\n}\n`,
  [`tests/test_${id}.c`]: `#include <stdio.h>\n#include "${id}.h"\n\nint main(void)\n{\n    ${id.replaceAll("-", "_")}_port_t port = {0};\n    ${id.replaceAll("-", "_")}_init(&port);\n    puts("${id}: ok");\n    return 0;\n}\n`,
};
for (const [name, content] of Object.entries(files)) {
  const file = path.join(target, name);
  fs.mkdirSync(path.dirname(file), {recursive: true});
  fs.writeFileSync(file, content);
}

manifest.units.push({
  id,
  kind,
  version: "0.1.0",
  path: directory,
  tags: ["todo"],
  summary,
  applications: ["todo"],
  compatibility: {language: "c99", alloc: false, float: false, blocking: false},
});
fs.writeFileSync(manifestPath, JSON.stringify(manifest, null, 2) + "\n");

let makefile = fs.readFileSync(makefilePath, "utf8");
if (!makefile.includes(`$(BUILD)/test_${id}`)) {
  const start = makefile.indexOf("TESTS :=");
  const end = makefile.indexOf("\n\n", start);
  const block = makefile.slice(start, end).split("\n");
  const last = block.pop();
  block.push(last.endsWith("\\") ? last : `${last} \\`);
  block.push(`\t$(BUILD)/test_${id}`);
  makefile = makefile.slice(0, start) + block.join("\n") + makefile.slice(end);
  const rule = `\n$(BUILD)/test_${id}: ${directory}/tests/test_${id}.c ${directory}/${id}.c | $(BUILD)\n\t$(CC) $(CFLAGS) -I${directory} $^ -o $@\n`;
  makefile = makefile.replace("\ntest: $(TESTS)", `${rule}\ntest: $(TESTS)`);
  fs.writeFileSync(makefilePath, makefile);
}
console.log(`new unit: ${directory}`);
console.log("next: fill the README/contract, run make test, then make site");
