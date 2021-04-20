CC=gcc

CFLAGS=-Wall -Wextra -pedantic -fstack-protector-all -pedantic -std=c99 -pthread
SANITY_FLAGS=-Wfloat-equal -Wshadow -Wpointer-arith

PREFIX ?= /usr

SOURCE=server.c keysmash.c

OUTPUT=server

ALL: $(OUTPUT)

debug: CFLAGS += -g -O0
debug: $(OUTPUT)

release: CFLAGS += -static -O3
release: $(OUTPUT)

$(OUTPUT): Makefile $(SOURCE)
	$(CC) $(CFLAGS) $(SANITY_FLAGS) $(SOURCE) -o $(OUTPUT)