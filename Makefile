CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -Werror -std=c11
relay: relay.c
	$(CC) $(CFLAGS) relay.c -o relay
test: relay
	python3 tests/integration.py
clean:
	rm -f relay
.PHONY: clean test
