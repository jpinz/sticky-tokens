CC ?= gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -I../../main -I../../components/cJSON

SRCS = ../../main/mtg_token.c ../../components/cJSON/cJSON.c test_mtg_token.c
BIN = test_mtg_token

.PHONY: test clean

test: $(BIN)
	./$(BIN)

$(BIN): $(SRCS)
	$(CC) $(CFLAGS) -o $(BIN) $(SRCS)

clean:
	rm -f $(BIN)
