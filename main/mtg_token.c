#include "mtg_token.h"

#include <string.h>

#include "cJSON.h"

static void copy_string_field(char *dest, size_t dest_len, const cJSON *object, const char *key)
{
    dest[0] = '\0';

    const cJSON *field = cJSON_GetObjectItemCaseSensitive(object, key);
    if (field == NULL || !cJSON_IsString(field) || field->valuestring == NULL) {
        return;
    }

    strncpy(dest, field->valuestring, dest_len - 1);
    dest[dest_len - 1] = '\0';
}

static void copy_colors_field(char *dest, size_t dest_len, const cJSON *object)
{
    dest[0] = '\0';

    const cJSON *colors = cJSON_GetObjectItemCaseSensitive(object, "colors");
    if (colors == NULL || !cJSON_IsArray(colors)) {
        return;
    }

    size_t written = 0;
    const cJSON *color = NULL;
    cJSON_ArrayForEach(color, colors) {
        if (!cJSON_IsString(color) || color->valuestring == NULL || color->valuestring[0] == '\0') {
            continue;
        }
        if (written + 1 >= dest_len) {
            break;
        }
        dest[written++] = color->valuestring[0];
    }
    dest[written] = '\0';
}

static bool card_is_token(const cJSON *card)
{
    const cJSON *layout = cJSON_GetObjectItemCaseSensitive(card, "layout");
    if (layout == NULL || !cJSON_IsString(layout) || layout->valuestring == NULL) {
        return false;
    }

    return strcmp(layout->valuestring, "token") == 0 || strcmp(layout->valuestring, "double_faced_token") == 0;
}

static void parse_card(const cJSON *card, mtg_token_t *out)
{
    memset(out, 0, sizeof(*out));
    copy_string_field(out->name, sizeof(out->name), card, "name");
    copy_string_field(out->type_line, sizeof(out->type_line), card, "type_line");
    copy_string_field(out->oracle_text, sizeof(out->oracle_text), card, "oracle_text");
    copy_string_field(out->power, sizeof(out->power), card, "power");
    copy_string_field(out->toughness, sizeof(out->toughness), card, "toughness");
    copy_colors_field(out->colors, sizeof(out->colors), card);
}

bool mtg_token_parse_search_response(const char *json, mtg_token_t *out, size_t max_out, size_t *out_count)
{
    if (json == NULL || out == NULL || out_count == NULL || max_out == 0) {
        return false;
    }

    *out_count = 0;

    cJSON *root = cJSON_Parse(json);
    if (root == NULL) {
        return false;
    }

    bool ok = false;
    const cJSON *object_type = cJSON_GetObjectItemCaseSensitive(root, "object");
    if (object_type != NULL && cJSON_IsString(object_type) && object_type->valuestring != NULL &&
        strcmp(object_type->valuestring, "error") == 0) {
        /* Scryfall returned an API error object, e.g. "no cards found". */
        goto done;
    }

    const cJSON *data = cJSON_GetObjectItemCaseSensitive(root, "data");
    if (data == NULL || !cJSON_IsArray(data)) {
        goto done;
    }

    size_t count = 0;
    const cJSON *card = NULL;
    cJSON_ArrayForEach(card, data) {
        if (count >= max_out) {
            break;
        }
        if (!cJSON_IsObject(card) || !card_is_token(card)) {
            continue;
        }
        parse_card(card, &out[count]);
        count++;
    }

    *out_count = count;
    ok = true;

done:
    cJSON_Delete(root);
    return ok;
}

size_t mtg_token_pick_random(size_t count, unsigned long random_value)
{
    if (count == 0) {
        return 0;
    }
    return (size_t)(random_value % count);
}
