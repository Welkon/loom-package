import assert from "node:assert/strict";
import {execFileSync} from "node:child_process";
import fs from "node:fs";
import os from "node:os";
import path from "node:path";
import test from "node:test";
import {fileURLToPath} from "node:url";
import {extractPorts, generate} from "./ports.mjs";

const here = path.dirname(fileURLToPath(import.meta.url));
const tool = path.join(here, "ports.mjs");

function fixture() {
  const root = fs.mkdtempSync(path.join(os.tmpdir(), "loom-ports-"));
  fs.mkdirSync(path.join(root, "drivers/sensor"), {recursive: true});
  fs.mkdirSync(path.join(root, "operators/filter"), {recursive: true});
  fs.writeFileSync(path.join(root, "loom.json"), JSON.stringify({
    schema: 1,
    units: [
      {id: "sensor", kind: "driver", path: "drivers/sensor", version: "0.1.0"},
      {id: "filter", kind: "operator", path: "operators/filter", version: "0.1.0"},
    ],
  }));
  fs.writeFileSync(path.join(root, "drivers/sensor/port.h"), `#ifndef SENSOR_PORT_H
#define SENSOR_PORT_H

typedef struct {
    void *context;
    /* multi-line declaration */
    uint8_t (*write)(void *context, uint8_t address,
                     const uint8_t *data, uint16_t length);
    uint32_t (*now_us)(void *context);
} sensor_port_t;

typedef struct {
    sensor_port_t port;
    uint8_t address;
} sensor_t;

#endif
`);
  fs.writeFileSync(path.join(root, "operators/filter/filter.h"), `typedef struct {
    int32_t sum;
} filter_t;
`);
  return root;
}

test("extracts port structs and normalizes multi-line declarations", () => {
  const root = fixture();
  const {ports, bare} = extractPorts(root);
  assert.equal(ports.length, 1);
  const port = ports[0];
  assert.equal(port.unit, "sensor");
  assert.equal(port.type, "sensor_port_t");
  assert.equal(port.header, "drivers/sensor/port.h");
  assert.equal(port.functions, 2);
  assert.deepEqual(port.fields.map(field => field.name), ["context", "write", "now_us"]);
  assert.equal(port.fields[1].signature,
    "uint8_t (*write)(void *context, uint8_t address, const uint8_t *data, uint16_t length)");
  assert.deepEqual(bare.map(unit => unit.id), ["filter"]);
  fs.rmSync(root, {recursive: true, force: true});
});

test("renders the struct in the document", () => {
  const root = fixture();
  const {text} = generate(root);
  assert.match(text, /1 port structs, 2 port functions, across 1 drivers/);
  assert.match(text, /typedef struct \{\n    void \*context;\n    uint8_t \(\*write\)\(/);
  assert.match(text, /Units without a port\n\n`filter`/);
  fs.rmSync(root, {recursive: true, force: true});
});

test("output is deterministic and the check mode catches drift", () => {
  const root = fixture();
  const out = path.join(root, "docs/ports.md");
  const run = (...args) => execFileSync(process.execPath, [tool, "--root", root, ...args], {stdio: "pipe"});
  run();
  const first = fs.readFileSync(out, "utf8");
  run();
  assert.equal(fs.readFileSync(out, "utf8"), first);
  assert.doesNotThrow(() => run("--check"));
  fs.appendFileSync(out, "\nstale\n");
  assert.throws(() => run("--check"), /stale/);
  fs.rmSync(root, {recursive: true, force: true});
});
