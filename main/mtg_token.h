#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MTG_TOKEN_NAME_LEN    64
#define MTG_TOKEN_TYPE_LEN    96
#define MTG_TOKEN_TEXT_LEN    512
#define MTG_TOKEN_PT_LEN      8
#define MTG_TOKEN_COLORS_LEN  8
#define MTG_TOKEN_MAX_RESULTS 40

/* A single Magic: The Gathering token card, as parsed out of the fields we
 * care about from the Scryfall API response. All strings are always
 * NUL-terminated, and empty ("") when the source data has no value for that
 * field (e.g. a token with no power/toughness). */
typedef struct {
    char name[MTG_TOKEN_NAME_LEN];
    char type_line[MTG_TOKEN_TYPE_LEN];
    char oracle_text[MTG_TOKEN_TEXT_LEN];
    char power[MTG_TOKEN_PT_LEN];
    char toughness[MTG_TOKEN_PT_LEN];
    /* Colors concatenated together, e.g. "WU" for a white/blue token, or "" for colorless. */
    char colors[MTG_TOKEN_COLORS_LEN];
} mtg_token_t;

/* Parses a Scryfall "list" JSON response (as returned by e.g.
 * https://api.scryfall.com/cards/search?q=is%3Atoken) and extracts up to
 * `max_out` token cards into `out`. Non-token entries (if any) are skipped.
 *
 * Returns true if `json` is a well-formed Scryfall list object, false
 * otherwise (e.g. malformed JSON, or an API error object). `*out_count` is
 * only valid when this function returns true, and is always <= max_out.
 */
bool mtg_token_parse_search_response(const char *json, mtg_token_t *out, size_t max_out, size_t *out_count);

/* Deterministically picks an index in the range [0, count) from
 * `random_value`. Returns 0 when count == 0, so callers can safely index
 * into a non-empty array without a separate zero-count check. */
size_t mtg_token_pick_random(size_t count, unsigned long random_value);

#ifdef __cplusplus
}
#endif
