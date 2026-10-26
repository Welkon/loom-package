# mfrc522

MFRC522 13.56 MHz RFID reader over SPI.

## Application scenarios

- A MIFARE UID reader.
- An access-control prototype.
- A tag-presence detector.

## Contract

```c
uint8_t mfrc522_init(mfrc522_t *reader, mfrc522_port_t port);
uint8_t mfrc522_request(mfrc522_t *reader, uint16_t *atqa);
uint8_t mfrc522_anticoll(mfrc522_t *reader, uint8_t uid[5]);
uint8_t mfrc522_read_uid(mfrc522_t *reader, uint8_t uid[4]);
```

- `init` performs a soft reset, antenna setup, and version check.
- `request` sends REQA and returns ATQA.
- `anticoll` sends the anticollision command and returns UID + BCC.
- `read_uid` combines both and checks BCC.
- Returns 1 on success, 0 on timeout, collision, or a bus error.

## Parameters

- SPI mode 0, up to 10 MHz.
- 3.3 V supply; the antenna pins are not 5 V tolerant.
- A 13.56 MHz card must be within a few centimeters.

## Memory and timing

- 4 bytes; one 25-byte command buffer is not required.
- A transceive waits up to 100 ms; the timer is configured for the MFRC522.

## Integration

```c
mfrc522_t reader;
uint8_t uid[4];
mfrc522_init(&reader, board_mfrc522_port());
if (mfrc522_read_uid(&reader, uid)) {
    show_uid(uid);
}
```

## Adapters

- The adapter drives NSS, SCK, MOSI, MISO, and the optional reset pin.
- Reference: LibDriver `mfrc522` (MIT) and the NXP MFRC522 datasheet.
- This implementation is original and covers UID reading only.

## When not to use

- MIFARE Classic block authentication/read/write; extend the command layer.
- A 125 kHz tag; use an EM4100 reader.

## Verification

`make test` covers the init register sequence, REQA, anticollision, BCC
validation, and a missing card.
