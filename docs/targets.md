# Targets

A port implementation needs three things: the port struct (here), the register
facts for the part (project or `emb-support`), and a host test. This page records
which targets have a reviewed path and how far each one was verified.

Reading the table:

- **host-verified** — the adapter compiles and passes the unit's own behavior
  assertions on the host, with a fake register file. No hardware.
- **bench-verified** — the same adapter ran on a board and the result was
  measured. Empty means nobody has claimed it.

| Target | Toolchain | Port style | Chip facts | Host-verified | Bench-verified |
| --- | --- | --- | --- | --- | --- |
| SCMCU SC8F072 | XC8 | `volatile unsigned char REG @ 0xNN` | `emb-support/.../sources/scmcu/SC8F072.md` (map) and `...-gpio.md` (behaviour) | `hx711` (`loom/examples/sc8f072-hx711`) | — |
| SCMCU SC8F072 | XC8 | same | `.../scmcu-sc8f072-registers.md` (timer, PWM, comparator, ADC) | — | — |
| Padauk PMS150G | Padauk mini-C | `PAC`/`PAPH` register bits | `.../padauk-pms15x-gpio.md` + `.../padauk/PMS150G.md` (map) | — (mini-C is not C, so the fake-register test does not apply; the worked adapter is `loom/examples/padauk-pms150g-hx711`) | — |
| Padauk other parts | Padauk mini-C | same | `.../padauk/<PART>.md` (127 generated maps) | — | — |
| Padauk PMB180B | Padauk mini-C | project SFR header | `.../padauk-pmb180b-registers.md` | — | — |
| ESP32-C3 | GCC (RISC-V) | ESP-IDF driver calls | `.../espressif-esp32-c3.md` | — | — |
| ARM Cortex-M | GCC/Clang | CMSIS `GPIO->BSRR` style | device header from the vendor pack | — | — |

The `emb-support` paths are relative to that repository root. They are chip
facts, not code: register names, addresses, and the behaviour that an adapter
must respect.

All 41 SC8 parts have a generated register map under
`emb-support/skills/mcu-debug/references/sources/scmcu/`, extracted from SCMCU
IDE `V2.00.16`. Hand-written notes add the behaviour the map cannot carry.

## Rules

- The catalog never carries a vendor header, a linker script, or a pin map.
  `docs/ports.md` is the only generated surface; `docs/adapters.md` is the only
  procedure.
- A chip fact belongs to `emb-support`, next to the other reviewed notes for
  that family. A board fact (which pin, which net) belongs to the project.
- A row is added to the host-verified column only with a runnable example that
  does not need hardware. A row is added to bench-verified only with a measured
  result and a date.
- One adapter per unit, not one per target. A second target for the same unit
  adds a `board/target.h`, not a second `ports_<unit>.c`.

## Adding a target

1. Write the chip facts note first: register names and addresses, and the
   three behaviours that bite (direction polarity, read-modify-write, analog
   select).
2. Add `board/target.h` with the compiler dialect and a `BOARD_HOST` branch.
3. Run `loom adapter <unit>`, then fill only the stub bodies.
4. Run the host test. Add the row above with `host-verified` set.
5. Leave `bench-verified` empty until a board run is recorded.
