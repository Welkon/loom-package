# nrf24l01

nRF24L01+ 2.4 GHz SPI transceiver core.

## Application scenarios

- A short-range sensor link.
- A remote control or RC channel.
- A low-power telemetry node.

## Contract

```c
uint8_t nrf24l01_init(nrf24l01_t *radio, nrf24l01_port_t port,
                      uint8_t channel, const uint8_t address[5],
                      uint8_t payload_length);
uint8_t nrf24l01_send(nrf24l01_t *radio, const uint8_t *data, uint8_t length);
uint8_t nrf24l01_receive(nrf24l01_t *radio, uint8_t *data, uint8_t capacity,
                         uint8_t *length);
```

- `init` configures a 5-byte address, 1 Mbps, 0 dBm, CRC, and auto-ack on pipe 0.
- `send` writes a payload and waits for TX_DS or MAX_RT.
- `receive` polls RX_DR and reads one payload if present.
- Returns 1 on success or a received packet, 0 on timeout or a bus error.

## Parameters

- Channel 0..125 (2400..2525 MHz).
- Payload 1..32 bytes; `payload_length` must match the receiver.
- Address is 5 bytes; both ends must use the same address.
- CE is driven by the adapter; SPI is mode 0.

## Memory and timing

- 16 bytes; one payload buffer of up to 32 bytes.
- A send waits up to 100 ms for TX_DS; retries are configured by SETUP_RETR.

## Integration

```c
static const uint8_t address[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
nrf24l01_t radio;
nrf24l01_init(&radio, board_nrf24l01_port(), 76, address, 4);
uint8_t payload[4] = {1, 2, 3, 4};
nrf24l01_send(&radio, payload, sizeof(payload));
```

## Adapters

- The adapter drives CSN, SCK, MOSI, MISO, and CE, and provides a 1 ms delay.
- Reference: LibDriver `nrf24l01` (MIT) and the Nordic nRF24L01+ datasheet.
- This implementation is original.

## When not to use

- A link that needs more than 32 bytes per packet; add a transport layer.
- A long-range or licensed band; use a different radio.

## Verification

`make test` covers the init registers, a transmit completion, a received
payload, and a missing packet.
