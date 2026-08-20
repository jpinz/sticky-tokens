#include <stdio.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_sleep.h"

#include "display.h"
#include "mtg_token.h"
#include "mtg_token_net.h"
#include "pin_config.h"
#include "wifi_manager.h"

static const char *TAG = "sticky_tokens";

/* Must run before anything else in app_main(): the reTerminal Sticky's main
 * power rail is gated by a latch circuit. Without this, the board loses
 * power shortly after the user releases the power button. */
static void power_on_hold(void)
{
    gpio_config_t config = {
        .pin_bit_mask = (1ULL << PIN_POWER_HOLD) | (1ULL << PIN_POWER_LOCK),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&config);

    gpio_hold_dis(PIN_POWER_HOLD);
    gpio_set_level(PIN_POWER_HOLD, 1);
    gpio_set_level(PIN_POWER_LOCK, 0);
    esp_rom_delay_us(10);
    gpio_set_level(PIN_POWER_LOCK, 1);
    esp_rom_delay_us(10);
    gpio_set_level(PIN_POWER_LOCK, 0);
}

static void show_message(const char *name, const char *type_line, const char *oracle_text)
{
    mtg_token_t message = {0};
    snprintf(message.name, sizeof(message.name), "%s", name);
    snprintf(message.type_line, sizeof(message.type_line), "%s", type_line);
    snprintf(message.oracle_text, sizeof(message.oracle_text), "%s", oracle_text);

    display_draw_token(&message);
    display_refresh();
}

static void go_to_sleep(void)
{
    display_sleep();

    /* Wake up either after the configured interval, or immediately if the
     * user presses the power button. */
    esp_sleep_enable_timer_wakeup((uint64_t)CONFIG_STICKY_REFRESH_INTERVAL_MINUTES * 60ULL * 1000000ULL);
    esp_sleep_enable_ext1_wakeup(1ULL << PIN_POWER_BTN, ESP_EXT1_WAKEUP_ANY_LOW);

    ESP_LOGI(TAG, "Entering deep sleep for %d minutes", CONFIG_STICKY_REFRESH_INTERVAL_MINUTES);
    esp_deep_sleep_start();
}

void app_main(void)
{
    power_on_hold();

    ESP_ERROR_CHECK(display_init());

    if (wifi_manager_connect(15000) != ESP_OK) {
        ESP_LOGE(TAG, "Could not connect to Wi-Fi");
        show_message("No Wi-Fi", "Check your network settings", "Configure Wi-Fi credentials via `idf.py menuconfig`.");
        go_to_sleep();
        return;
    }

    mtg_token_t token;
    if (!mtg_token_fetch_random(&token)) {
        ESP_LOGE(TAG, "Could not fetch a token from Scryfall");
        show_message("No token found", "", "Could not reach the Scryfall API. Will retry later.");
        go_to_sleep();
        return;
    }

    ESP_LOGI(TAG, "Displaying token: %s", token.name);
    display_draw_token(&token);
    display_refresh();

    go_to_sleep();
}
