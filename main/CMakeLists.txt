idf_component_register(
    SRCS "main.c"
         "wifi_manager.c"
         "mtg_token.c"
         "mtg_token_net.c"
         "display.c"
    INCLUDE_DIRS "."
    REQUIRES cJSON
             seeed_epaper
             debug_logging
             esp_wifi
             esp_netif
             esp_event
             esp_http_client
             mbedtls
             nvs_flash
             esp_driver_gpio
             esp_driver_spi
             esp_hw_support
             esp_rom
             esp_timer
             heap
)
