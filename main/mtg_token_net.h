#pragma once

#include <stdbool.h>

#include "mtg_token.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Base URL for the Scryfall "random card" search endpoint, scoped to token
 * cards only. Scryfall's own `/cards/random` endpoint does not support
 * filtering by type, so we instead pull a page of token search results and
 * pick one at random locally (see mtg_token_pick_random). */
#define MTG_TOKEN_SCRYFALL_URL \
    "https://api.scryfall.com/cards/search?q=is%3Atoken&order=random"

/* Fetches a page of Magic: The Gathering token cards from the Scryfall API
 * over HTTPS and fills `out` with one picked at random. Requires that Wi-Fi
 * is already connected. Returns true on success. */
bool mtg_token_fetch_random(mtg_token_t *out);

#ifdef __cplusplus
}
#endif
