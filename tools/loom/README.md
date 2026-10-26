# loom catalog tools

```sh
node tools/loom/new-unit.mjs my-driver --kind driver --summary "One-line purpose"
node tools/loom/check-catalog.mjs
make test
make site
```

`new-unit` creates the README, header, C source, host test, `loom.json` entry,
and Makefile wiring. Fill the contract and tests before opening a change.
`check-catalog` verifies the files and metadata; it does not replace `make test`.
