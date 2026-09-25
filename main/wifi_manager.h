#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Initializes NVS, netif, and the Wi-Fi driver in station mode, then blocks
 * until either an IP address has been obtained or `timeout_ms` elapses.
 * SSID/password are read from Kconfig (see Kconfig.projbuild).
 *
 * Returns ESP_OK if connected, ESP_ERR_TIMEOUT on timeout, or another
 * esp_err_t on lower-level failure.
 */
esp_err_t wifi_manager_connect(int timeout_ms);

#ifdef __cplusplus
}
#endif
