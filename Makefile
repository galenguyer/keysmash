CC=gcc

CFLAGS=-Wall -Wextra -pedantic -fstack-protector-all -pedantic -std=c99 -lpthread
SANITY_FLAGS=-Wfloat-equal -Wshadow -Wpointer-arith

PREFIX ?= /usr

SOURCE=gay.c

OUTPUT=gay

ALL: $(OUTPUT)

debug: CFLAGS += -g -O0
debug: $(OUTPUT)

release: CFLAGS += -static -O3
release: $(OUTPUT)

$(OUTPUT): Makefile $(SOURCE)

server: server.c
	$(CC) -o $@ $(CFLAGS) $(SANITY_FLAGS) $<
