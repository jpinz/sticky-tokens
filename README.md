# sticky-tokens

Custom firmware for the [Seeed Studio reTerminal Sticky](https://www.seeedstudio.com/sticky/docs/)
(ESP32-S3, 3.97" 800x480 E-Ink) that displays a random Magic: The Gathering
token card, fetched from the [Scryfall API](https://scryfall.com/docs/api).

On boot, the device connects to Wi-Fi, requests a page of token cards from
Scryfall, picks one at random, renders its name, type line, power/toughness,
colors, and oracle text to the E-Ink panel, then deep-sleeps until the next
refresh interval (or until the power button is pressed).

## Hardware

- SoC: ESP32-S3R8 (8 MB octal PSRAM), 32 MB QSPI flash
- Display: 3.97" monochrome E-Ink, SSD1677 controller, 800x480

## Project layout

```
CMakeLists.txt          top-level ESP-IDF project file
sdkconfig.defaults       board-specific build defaults (PSRAM, flash size, ...)
main/
  main.c                 app entry: power latch, Wi-Fi, fetch, render, sleep
  pin_config.h            GPIO map for the reTerminal Sticky
  wifi_manager.[ch]      Wi-Fi station connect helper
  mtg_token.[ch]         Scryfall JSON parsing + random token selection (host-testable)
  mtg_token_net.[ch]     Scryfall HTTPS client (ESP-IDF only)
  display.[ch]           Frame buffer, text rendering, E-Ink refresh
  font8x8_basic.h        Public-domain 8x8 bitmap font
  Kconfig.projbuild      Wi-Fi credentials and refresh interval settings
components/
  cJSON/                 Vendored cJSON (MIT)
  seeed_epaper/          Vendored SSD1677/UC8179 panel driver (MIT)
  debug_logging/         Vendored debug logging helpers (MIT)
test/host/               Host-side (gcc) unit tests for mtg_token.c
```

See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for attribution of
vendored third-party code.

## Building and flashing

Requires [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/get-started/)
v5.4+ with the `esp32s3` target installed.

```bash
idf.py set-target esp32s3
idf.py menuconfig   # under "Sticky Tokens Configuration": set Wi-Fi SSID/password
idf.py build
idf.py -p PORT flash monitor
```

Wi-Fi SSID/password and the refresh interval (in minutes) can also be set
directly in `sdkconfig.defaults` or via `idf.py menuconfig` under
"Sticky Tokens Configuration".

## Running the host-side unit tests

The Scryfall JSON parsing and random-selection logic in `main/mtg_token.c` has
no ESP-IDF dependencies, so it can be built and tested on a regular machine
with `gcc`:

```bash
cd test/host
make test
```

## Disclaimer

This project is not produced by, endorsed by, or affiliated with Wizards of
the Coast. Magic: The Gathering and its card data are trademarks/property of
Wizards of the Coast; card data is fetched live from Scryfall and is not
redistributed by this repository.
