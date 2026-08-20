/* Host-side unit tests for the pure MTG token parsing/selection logic in
 * main/mtg_token.c. Build & run with `make test` from this directory. */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "mtg_token.h"

static const char *k_sample_response =
    "{"
    "  \"object\": \"list\","
    "  \"total_cards\": 2,"
    "  \"has_more\": false,"
    "  \"data\": ["
    "    {"
    "      \"object\": \"card\","
    "      \"layout\": \"normal\","
    "      \"name\": \"Lightning Bolt\","
    "      \"type_line\": \"Instant\","
    "      \"oracle_text\": \"Lightning Bolt deals 3 damage to any target.\""
    "    },"
    "    {"
    "      \"object\": \"card\","
    "      \"layout\": \"token\","
    "      \"name\": \"Soldier\","
    "      \"type_line\": \"Token Creature — Soldier\","
    "      \"oracle_text\": \"\","
    "      \"power\": \"1\","
    "      \"toughness\": \"1\","
    "      \"colors\": [\"W\"]"
    "    },"
    "    {"
    "      \"object\": \"card\","
    "      \"layout\": \"double_faced_token\","
    "      \"name\": \"Wolf // Back\","
    "      \"type_line\": \"Token Creature — Wolf\","
    "      \"oracle_text\": \"\","
    "      \"power\": \"2\","
    "      \"toughness\": \"2\","
    "      \"colors\": []"
    "    }"
    "  ]"
    "}";

static const char *k_error_response =
    "{\"object\": \"error\", \"code\": \"not_found\", \"details\": \"No cards found.\"}";

static void test_parses_only_token_cards(void)
{
    mtg_token_t tokens[MTG_TOKEN_MAX_RESULTS];
    size_t count = 0;

    bool ok = mtg_token_parse_search_response(k_sample_response, tokens, MTG_TOKEN_MAX_RESULTS, &count);

    assert(ok);
    assert(count == 2);
    assert(strcmp(tokens[0].name, "Soldier") == 0);
    assert(strcmp(tokens[0].type_line, "Token Creature — Soldier") == 0);
    assert(strcmp(tokens[0].power, "1") == 0);
    assert(strcmp(tokens[0].toughness, "1") == 0);
    assert(strcmp(tokens[0].colors, "W") == 0);
    assert(strcmp(tokens[1].name, "Wolf // Back") == 0);
    assert(strcmp(tokens[1].colors, "") == 0);

    printf("PASS test_parses_only_token_cards\n");
}

static void test_respects_max_out(void)
{
    mtg_token_t tokens[1];
    size_t count = 0;

    bool ok = mtg_token_parse_search_response(k_sample_response, tokens, 1, &count);

    assert(ok);
    assert(count == 1);
    assert(strcmp(tokens[0].name, "Soldier") == 0);

    printf("PASS test_respects_max_out\n");
}

static void test_rejects_error_response(void)
{
    mtg_token_t tokens[MTG_TOKEN_MAX_RESULTS];
    size_t count = 123;

    bool ok = mtg_token_parse_search_response(k_error_response, tokens, MTG_TOKEN_MAX_RESULTS, &count);

    assert(!ok);
    assert(count == 0);

    printf("PASS test_rejects_error_response\n");
}

static void test_rejects_malformed_json(void)
{
    mtg_token_t tokens[MTG_TOKEN_MAX_RESULTS];
    size_t count = 0;

    bool ok = mtg_token_parse_search_response("not json", tokens, MTG_TOKEN_MAX_RESULTS, &count);

    assert(!ok);

    printf("PASS test_rejects_malformed_json\n");
}

static void test_rejects_null_arguments(void)
{
    mtg_token_t tokens[MTG_TOKEN_MAX_RESULTS];
    size_t count = 0;

    assert(!mtg_token_parse_search_response(NULL, tokens, MTG_TOKEN_MAX_RESULTS, &count));
    assert(!mtg_token_parse_search_response(k_sample_response, NULL, MTG_TOKEN_MAX_RESULTS, &count));
    assert(!mtg_token_parse_search_response(k_sample_response, tokens, 0, &count));
    assert(!mtg_token_parse_search_response(k_sample_response, tokens, MTG_TOKEN_MAX_RESULTS, NULL));

    printf("PASS test_rejects_null_arguments\n");
}

static void test_pick_random_in_range(void)
{
    assert(mtg_token_pick_random(0, 42) == 0);
    assert(mtg_token_pick_random(1, 42) == 0);
    assert(mtg_token_pick_random(5, 0) == 0);
    assert(mtg_token_pick_random(5, 5) == 0);
    assert(mtg_token_pick_random(5, 7) == 2);

    /* The result must always be a valid index into a `count`-sized array,
     * regardless of how large `random_value` is. */
    for (unsigned long random_value = 0; random_value < 1000; random_value += 37) {
        assert(mtg_token_pick_random(5, random_value) < 5);
    }
    assert(mtg_token_pick_random(5, (unsigned long)-1) < 5);

    printf("PASS test_pick_random_in_range\n");
}

int main(void)
{
    test_parses_only_token_cards();
    test_respects_max_out();
    test_rejects_error_response();
    test_rejects_malformed_json();
    test_rejects_null_arguments();
    test_pick_random_in_range();

    printf("All mtg_token tests passed.\n");
    return 0;
}
