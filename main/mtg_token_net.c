#include "mtg_token_net.h"

#include <stdlib.h>
#include <string.h>

#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_random.h"

static const char *TAG = "mtg_token_net";

/* Scryfall responses for a token search page are well under this size in
 * practice; if a response is truncated, parsing simply fails safely. */
#define HTTP_RESPONSE_BUFFER_LEN (128 * 1024)

typedef struct {
    char *buffer;
    size_t len;
    size_t capacity;
} http_response_ctx_t;

static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    http_response_ctx_t *ctx = (http_response_ctx_t *)evt->user_data;

    if (evt->event_id == HTTP_EVENT_ON_DATA && ctx != NULL) {
        size_t space_left = ctx->capacity - ctx->len - 1;
        size_t to_copy = (size_t)evt->data_len < space_left ? (size_t)evt->data_len : space_left;
        if (to_copy > 0) {
            memcpy(ctx->buffer + ctx->len, evt->data, to_copy);
            ctx->len += to_copy;
            ctx->buffer[ctx->len] = '\0';
        }
    }

    return ESP_OK;
}

bool mtg_token_fetch_random(mtg_token_t *out)
{
    if (out == NULL) {
        return false;
    }

    http_response_ctx_t ctx = {
        .buffer = malloc(HTTP_RESPONSE_BUFFER_LEN),
        .len = 0,
        .capacity = HTTP_RESPONSE_BUFFER_LEN,
    };
    if (ctx.buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate HTTP response buffer");
        return false;
    }
    ctx.buffer[0] = '\0';

    esp_http_client_config_t config = {
        .url = MTG_TOKEN_SCRYFALL_URL,
        .event_handler = http_event_handler,
        .user_data = &ctx,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .timeout_ms = 15000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to init HTTP client");
        free(ctx.buffer);
        return false;
    }

    esp_err_t err = esp_http_client_perform(client);
    bool ok = false;

    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        if (status == 200) {
            mtg_token_t tokens[MTG_TOKEN_MAX_RESULTS];
            size_t count = 0;
            if (mtg_token_parse_search_response(ctx.buffer, tokens, MTG_TOKEN_MAX_RESULTS, &count) && count > 0) {
                size_t index = mtg_token_pick_random(count, esp_random());
                *out = tokens[index];
                ok = true;
            } else {
                ESP_LOGE(TAG, "No tokens found in Scryfall response");
            }
        } else {
            ESP_LOGE(TAG, "Scryfall request failed with HTTP status %d", status);
        }
    } else {
        ESP_LOGE(TAG, "Scryfall request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    free(ctx.buffer);
    return ok;
}
