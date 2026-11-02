---
name: Package request
about: Propose a new driver or algorithm for the loom catalog
title: "[package] "
labels: needs-triage, package-request
assignees: ""
---

**Package id**

`example-driver` — lowercase kebab-case, stable once published.

**Kind**

`driver` (protocol or state machine behind a port) or `operator` (pure computation).

**Device or source**

Part number, datasheet, published algorithm, or the real project whose behavior
is the reference. Name the license of that reference.

**Use case**

What should a project be able to do with this package? Describe the product
situation, not just the chip.

**Why an existing unit does not fit**

Name the closest units and the concrete gap. "Almost the same, but cleaner"
is not enough; a near-duplicate is rejected.

**Contract sketch**

Inputs, outputs, buffer sizes, integer or floating point, blocking or
non-blocking, and what the caller owns.

**Hardware facts**

Bus, voltage, timing, package. Mark what must be measured on a bench rather
than read from a datasheet.

**License and provenance**

License of the reference implementation, or "datasheet only". GPL and
vendor-confidential sources cannot become catalog units.

**Checks**

- [ ] I searched `loom.json` and the unit folders.
- [ ] I can name the behavior reference and its license.
- [ ] I can state the contract and tests without inventing hardware facts.
