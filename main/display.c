#include "display.h"

#include <stdio.h>
#include <string.h>

#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_log.h"

#include "epaper_panel.h"
#include "font8x8_basic.h"
#include "pin_config.h"

static const char *TAG = "display";

#define FB_STRIDE_BYTES (DISPLAY_WIDTH / 8)
#define FB_SIZE_BYTES   (FB_STRIDE_BYTES * DISPLAY_HEIGHT)

static seeed_epaper_panel_handle_t s_panel;
static uint8_t *s_framebuffer;

/* Draws (or erases) a single pixel. MONO1_MSB: a set bit is white, a clear
 * bit is black (see components/seeed_epaper/README.md). */
static void draw_pixel(int x, int y, bool black)
{
    if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT) {
        return;
    }

    uint8_t *byte = &s_framebuffer[y * FB_STRIDE_BYTES + x / 8];
    uint8_t mask = (uint8_t)(0x80U >> (x % 8));

    if (black) {
        *byte &= (uint8_t)~mask;
    } else {
        *byte |= mask;
    }
}

static void draw_char(int x, int y, char c, int scale)
{
    if ((unsigned char)c >= 128) {
        c = '?';
    }

    for (int row = 0; row < 8; row++) {
        uint8_t bits = (uint8_t)font8x8_basic[(unsigned char)c][row];
        for (int col = 0; col < 8; col++) {
            if ((bits & (1 << col)) == 0) {
                continue;
            }
            for (int sy = 0; sy < scale; sy++) {
                for (int sx = 0; sx < scale; sx++) {
                    draw_pixel(x + col * scale + sx, y + row * scale + sy, true);
                }
            }
        }
    }
}

/* Draws word-wrapped text starting at (x, y), constrained to `wrap_width`
 * pixels, and returns the y coordinate immediately below the last line
 * drawn (so callers can stack multiple text blocks). Honors '\n' as an
 * explicit line break (Scryfall oracle text uses these between abilities). */
static int draw_text_wrapped(int x, int y, int wrap_width, int scale, const char *text)
{
    const int char_width = 8 * scale;
    const int line_height = 10 * scale;
    const int chars_per_line = wrap_width > 0 ? wrap_width / char_width : 1;

    int cursor_y = y;
    size_t len = strlen(text);
    size_t pos = 0;

    while (pos < len) {
        /* Skip leading spaces on a fresh line. */
        while (pos < len && text[pos] == ' ') {
            pos++;
        }

        size_t line_start = pos;
        size_t last_space = 0;
        bool have_space = false;
        size_t col = 0;

        while (pos < len && text[pos] != '\n' && col < (size_t)chars_per_line) {
            if (text[pos] == ' ') {
                last_space = pos;
                have_space = true;
            }
            pos++;
            col++;
        }

        size_t line_end;
        size_t next_pos;
        if (pos >= len || text[pos] == '\n') {
            /* Line ended naturally (end of text or explicit newline). */
            line_end = pos;
            next_pos = (pos < len) ? pos + 1 : pos;
        } else if (have_space) {
            /* Wrap at the last space so we don't split a word. */
            line_end = last_space;
            next_pos = last_space + 1;
        } else {
            /* A single word longer than the line; hard-break it. */
            line_end = pos;
            next_pos = pos;
        }

        int draw_x = x;
        for (size_t i = line_start; i < line_end; i++) {
            draw_char(draw_x, cursor_y, text[i], scale);
            draw_x += char_width;
        }

        cursor_y += line_height;
        pos = next_pos;
    }

    return cursor_y;
}

esp_err_t display_init(void)
{
    s_framebuffer = heap_caps_malloc(FB_SIZE_BYTES, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (s_framebuffer == NULL) {
        s_framebuffer = heap_caps_malloc(FB_SIZE_BYTES, MALLOC_CAP_8BIT);
    }
    if (s_framebuffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate %d byte frame buffer", FB_SIZE_BYTES);
        return ESP_ERR_NO_MEM;
    }

    spi_bus_config_t bus_config = {
        .mosi_io_num = PIN_EPD_MOSI,
        .miso_io_num = PIN_EPD_MISO,
        .sclk_io_num = PIN_EPD_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = FB_SIZE_BYTES,
    };
    esp_err_t err = spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "spi_bus_initialize failed: %s", esp_err_to_name(err));
        return err;
    }

    spi_device_interface_config_t dev_config = {
        .clock_speed_hz = 10 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = PIN_EPD_CS,
        .queue_size = 4,
    };
    spi_device_handle_t spi_handle;
    err = spi_bus_add_device(SPI2_HOST, &dev_config, &spi_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "spi_bus_add_device failed: %s", esp_err_to_name(err));
        return err;
    }

    seeed_epaper_panel_config_t panel_config = {
        .spi_handle = spi_handle,
        .pin_dc = PIN_EPD_DC,
        .pin_rst = PIN_EPD_RST,
        .pin_busy = PIN_EPD_BUSY,
        .pin_enable = PIN_EPD_EN,
        .busy_timeout_ms = 10000,
        .reset_low_ms = 10,
        .reset_high_ms = 10,
        .busy_level = 1,
        .enable_level = 1,
        .mirror_x = false,
    };
    err = seeed_epaper_new_panel(SEEED_EPAPER_PANEL_SSD1677, &panel_config, &s_panel);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "seeed_epaper_new_panel failed: %s", esp_err_to_name(err));
        return err;
    }

    display_clear();
    return ESP_OK;
}

void display_clear(void)
{
    memset(s_framebuffer, 0xFF, FB_SIZE_BYTES);
}

void display_draw_token(const mtg_token_t *token)
{
    const int margin = 20;
    const int content_width = DISPLAY_WIDTH - 2 * margin;

    display_clear();

    int y = margin;
    y = draw_text_wrapped(margin, y, content_width, 4, token->name);
    y += 10;
    y = draw_text_wrapped(margin, y, content_width, 2, token->type_line);
    y += 10;

    if (token->power[0] != '\0' || token->toughness[0] != '\0') {
        char pt[32];
        snprintf(pt, sizeof(pt), "%s/%s", token->power, token->toughness);
        y = draw_text_wrapped(margin, y, content_width, 3, pt);
        y += 10;
    }

    if (token->colors[0] != '\0') {
        char colors_line[MTG_TOKEN_COLORS_LEN + 16];
        snprintf(colors_line, sizeof(colors_line), "Colors: %s", token->colors);
        y = draw_text_wrapped(margin, y, content_width, 2, colors_line);
        y += 10;
    }

    draw_text_wrapped(margin, y, content_width, 2, token->oracle_text);
}

esp_err_t display_refresh(void)
{
    const seeed_epaper_area_t area = {0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT};

    esp_err_t err = seeed_epaper_panel_prepare(s_panel, SEEED_EPAPER_REFRESH_FULL);
    if (err != ESP_OK) {
        return err;
    }

    err = seeed_epaper_panel_write_bitmap(s_panel, &area, s_framebuffer, FB_STRIDE_BYTES,
                                           SEEED_EPAPER_PIXEL_FORMAT_MONO1_MSB, SEEED_EPAPER_REFRESH_FULL);
    if (err != ESP_OK) {
        return err;
    }

    return seeed_epaper_panel_commit(s_panel, &area, SEEED_EPAPER_REFRESH_FULL);
}

esp_err_t display_sleep(void)
{
    return seeed_epaper_panel_sleep(s_panel);
}
