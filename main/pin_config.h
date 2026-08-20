#pragma once

/* reTerminal Sticky (ESP32-S3) pin map.
 *
 * Sourced from the Seeed Studio hardware overview, cross-checked against the
 * `sticky-2048` reference application in the Playground registry
 * (https://github.com/Seeed-Projects/reterminal-sticky-playground-registry).
 * Only the pins this firmware currently uses (power latch, buttons, and the
 * E-Ink panel) are defined here.
 */

// Power latch (must be asserted at the start of app_main(), see main.c)
#define PIN_POWER_BTN       4
#define PIN_POWER_HOLD      45
#define PIN_POWER_LOCK      46

// Buttons
#define PIN_BTN_UP          5
#define PIN_BTN_DOWN        6
#define PIN_BTN_OK          PIN_POWER_BTN

// E-paper SSD1677 (SPI2)
#define PIN_EPD_MOSI        14
#define PIN_EPD_CLK         13
#define PIN_EPD_MISO        12
#define PIN_EPD_CS          15
#define PIN_EPD_DC          16
#define PIN_EPD_RST         17
#define PIN_EPD_BUSY        18
#define PIN_EPD_EN          47
