# AGENTS.md

## Scope

This repository is the loom catalog: reusable C drivers and algorithms with
host tests, provenance, and a static website. It is not a project template.
Keep every unit general; board wiring stays in a project adapter.

## AI workflow

1. **Search first.** Look in `loom.json` and the unit directories before
   writing a new unit. If an existing unit is close, extend its adapter in the
   consuming project instead of creating a near-duplicate here.
2. **Open a gap.** A new unit needs a concrete use case that is not covered by
   an existing unit. State the gap in the change description.
3. **Scaffold.** Run:

   ```sh
   node tools/loom/new-unit.mjs <id> --kind driver|operator --summary "One line"
   ```

4. **Fill the contract.** Complete `README.md`, the header, the C source, and
   deterministic host tests. Replace every scaffold placeholder.
5. **Verify.**

   ```sh
   make test
   node tools/loom/check-catalog.mjs
   node --test site/build.test.mjs
   ```

6. **Report.** State the source behavior, license, tests, and any hardware
   behavior still unverified on a bench.

## Unit rules

- C99, no dynamic allocation, no floating point unless the contract requires it.
- The unit owns computation and protocol state. It must not name an MCU,
  register, pin, clock, or board net. Put those in a port or adapter.
- Every port is a function-pointer struct with a `void *context`.
- Every unit has a `README.md` with purpose, application scenarios, contract,
  parameters, memory and timing, integration, adapters, when not to use, and
  verification.
- Every unit has host tests with a fake port. A test that only compiles is not
  enough.
- Keep `compatibility` accurate: language, allocation, floating point, blocking.
- Do not add a unit to grow the count. A smaller catalog with clear contracts
  is better than a large one with duplicated behavior.

## Provenance and licensing

- Use a real project, a vendor datasheet, or a published algorithm as the
  behavior reference. Name it in the README.
- MIT, BSD, Apache-2.0, zlib, and public-domain references are acceptable.
  Do not copy source from a GPL or vendor-supplied project into this catalog.
- Rewrite the implementation. Record the behavior reference and the license;
  do not copy vendor code or comments.
- Do not add secrets, tokens, generated `site/dist`, or build artifacts.
- Do not bump the catalog version or create a tag. The maintainer does that
  after review.

## Stop conditions

Stop and report instead of guessing when:

- the only source is a vendor file with unclear redistribution terms;
- the unit needs a dependency or a runtime that the catalog does not have;
- the behavior depends on a board measurement that is not available;
- the contract or the tests cannot be stated without inventing hardware facts.

## Consumer loop

Consumers discover on <https://loom.pcbsmt.me>, install with
`loom add <id>`, check with `loom verify`, and block unprovenanced drivers with
`loom guard --staged`. A catalog change is complete only when the unit, tests,
metadata, and site data agree.
