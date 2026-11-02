## What changed

<!-- One or two sentences. Name the unit and the behavior, not the file list. -->

## Gap

<!-- Which concrete use case is not covered by an existing unit? Name the closest units. -->

## Provenance

<!-- Behavior reference (project, datasheet, or published algorithm) and its license. -->

## Checks

- [ ] I searched `loom.json` and the unit folders; this is not a near-duplicate.
- [ ] The unit README states purpose, application scenarios, contract, parameters, memory, timing, integration, adapters, when not to use, and verification.
- [ ] Host tests use a fake port and fail without this change; a compile-only test is not enough.
- [ ] Provenance and license are named; no GPL or vendor-supplied source was copied.
- [ ] `make test`, `node tools/loom/check-catalog.mjs`, and `node --test site/build.test.mjs` pass.
- [ ] No catalog version bump, no tag, no secret, no generated `site/dist` in this change.

## Bench verification

<!-- What is still unverified on hardware? Write "none" if nothing applies. -->
