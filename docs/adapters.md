# Adapters

An adapter is the only code that names a pin, a register, a clock, or a vendor
header. Every catalog unit stays portable; the project owns the adapter and the
chip facts behind it.

[`ports.md`](ports.md) lists every `*_port_t` struct and its functions. Read the
unit header before writing an adapter; do not guess a function name.

## Project layout

```
board/
  README.md      chip, toolchain, header source, units verified on this board
  target.h       the only file that includes a vendor or SFR header
  vendor/        optional, vendor header kept verbatim with provenance
  ports.c        every board_<unit>_port() builder
  ports_<id>.c   the same builders, one file per unit, from `loom adapter`
  tests/
    fake_sfr.h   host build: the register block as plain memory
```

`board/README.md` is the entry point for a person or an agent. It records:

- the exact part number and package;
- the compiler and its dialect;
- where the SFR header comes from, and whether it is vendored or installed with
  the toolchain;
- which units have been run on a bench, and which are only host-tested.

## target.h

Keep one include point. Everything above it speaks `BOARD_*`, everything below
it speaks the chip.

```c
/* board/target.h */
#ifndef BOARD_TARGET_H
#define BOARD_TARGET_H

#if defined(BOARD_HEADER_VENDOR)
#include "vendor/part_number.h"      /* verbatim, provenance in board/README.md */
#else
/* Minimal shim: only the SFRs this board uses, from the datasheet. */
extern volatile unsigned char BOARD_REG_PORT;
extern volatile unsigned char BOARD_REG_DIR;
#define BOARD_PIN_MASK(pin) (1u << (pin))
#endif

#define BOARD_SET(pin, level) \
    ((level) ? (BOARD_REG_PORT |= BOARD_PIN_MASK(pin)) \
             : (BOARD_REG_PORT &= (unsigned char)~BOARD_PIN_MASK(pin)))
#define BOARD_GET(pin) ((BOARD_REG_PORT >> (pin)) & 1u)

void board_delay_us(unsigned long microseconds);
void board_delay_ms(unsigned long milliseconds);

#endif
```

A shim is smaller and clearer than a vendor tree, and it records only what the
board uses. Take the names from the datasheet; do not copy vendor source.

## ports.c

Start from `loom adapter <unit>`, which writes `board/ports_<unit>.h` and
`board/ports_<unit>.c` from the installed unit header: one stub per port
function with the exact signature, plus the `board_<unit>_port()` builder.
`loom adapter <unit> --check` fails when a unit update adds a function that the
adapter has no assignment for; hand edits are preserved. The command writes no
MCU name and does not create `target.h`.

One builder per unit. The `context` pointer stays unused when the board has a
single instance; otherwise point it at a bus or device descriptor.

```c
/* board/ports.c */
#include "target.h"
#include "hx711.h"

static uint8_t hx711_read_dout(void *context)
{
    (void)context;
    return BOARD_GET(BOARD_HX711_DOUT);
}

static void hx711_set_sck(void *context, uint8_t level)
{
    (void)context;
    BOARD_SET(BOARD_HX711_SCK, level);
}

static void hx711_delay_us(void *context, uint16_t microseconds)
{
    (void)context;
    board_delay_us(microseconds);
}

hx711_port_t board_hx711_port(void)
{
    hx711_port_t port;
    port.context = 0;
    port.read_dout = hx711_read_dout;
    port.set_sck = hx711_set_sck;
    port.delay_us = hx711_delay_us;
    return port;
}
```

`board_<unit>_port()` is the name the unit README uses in its integration
block. Keep it stable: call sites and tests refer to it.

## Host test

An adapter does not need hardware to be compiled and exercised. On the host,
build `target.h` against a plain array:

```c
/* board/tests/fake_sfr.h */
#include <stdint.h>
extern volatile uint8_t board_sfr[256];
#define BOARD_REG_PORT board_sfr[0x80]
#define BOARD_REG_DIR  board_sfr[0x86]
```

Then link `ports.c` with the unit's own host test. The fake port in
`drivers/<unit>/tests/` already defines the expected behavior; the adapter is
the implementation under test. This catches a wrong pin mask or an inverted
level before the board exists.

## Checklist

- [ ] `board/README.md` names the part, the toolchain, and the header source.
- [ ] `docs/targets.md` has a row for the part, with the verification level you
      actually reached.
- [ ] `board/target.h` is the only file that includes a vendor or SFR header.
- [ ] Every port function from [`ports.md`](ports.md) is implemented, and
      returns the unit's documented error codes.
- [ ] `board/ports.c` compiles on the host against `fake_sfr.h`.
- [ ] Blocking calls respect the unit's documented timing budget.
- [ ] `board/README.md` separates bench-verified units from host-only units.
