CC=musl-gcc

CFLAGS=-Wall -Wextra -pedantic -fstack-protector-all -Wstack-protector -pedantic -std=c99 -static
SANITY_FLAGS=-Wfloat-equal -Wshadow -Wpointer-arith

PREFIX ?= /usr

SOURCE=server.c keysmash.c

OUTPUT=server

ALL: $(OUTPUT)

debug: CFLAGS += -g3 -O0 -fno-omit-frame-pointer
debug: $(OUTPUT)

release: CFLAGS += -static -Os -s -ffunction-sections -fdata-sections -Wl,--gc-sections -fomit-frame-pointer
release: $(OUTPUT)

$(OUTPUT): Makefile $(SOURCE)
	$(CC) $(CFLAGS) $(SANITY_FLAGS) $(SOURCE) -o $(OUTPUT)
