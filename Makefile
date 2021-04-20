CC=gcc

CFLAGS=-Wall -Wextra -pedantic -fstack-protector-all -pedantic -std=c99 -lpthread
SANITY_FLAGS=-Wfloat-equal -Wshadow -Wpointer-arith

PREFIX ?= /usr

debug: CFLAGS += -g -O0
release: CFLAGS += -static -O3

server: server.c keysmash.c
	$(CC) -o $@ $(CFLAGS) $(SANITY_FLAGS) $<
