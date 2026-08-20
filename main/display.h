#pragma once

#include "esp_err.h"

#include "mtg_token.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DISPLAY_WIDTH  800
#define DISPLAY_HEIGHT 480

/* Initializes SPI2, the SSD1677 panel driver, and the in-memory frame
 * buffer used to compose a screen before pushing it to the panel. */
esp_err_t display_init(void);

/* Fills the frame buffer with white. */
void display_clear(void);

/* Lays out a token's name, type line, power/toughness, and oracle text into
 * the frame buffer, word-wrapping the oracle text to fit the panel width. */
void display_draw_token(const mtg_token_t *token);

/* Pushes the frame buffer to the panel and performs a full refresh. */
esp_err_t display_refresh(void);

/* Puts the panel into its low-power sleep mode. The last refreshed image
 * remains visible while the panel (and MCU) are asleep. */
esp_err_t display_sleep(void);

#ifdef __cplusplus
}
#endif
