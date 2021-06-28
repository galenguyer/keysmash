#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void keysmash(char* output, unsigned length) {
    char keys[16] = { 'a', 's', 'd', 'f', 'g', 'j', 'k', 'l', 'a', 's', 'd', 'f', 'j', 'k', 'l', ';' };
    // Special case for this, otherwise we're writing out of bounds!
    if (length == 1) {
        output[0] = 'a';
        return;
    }
    output[0] = 'a';
    output[1] = 's';
    while (length != 2) {
        // random() is a 32 bit int. we have 16 possibilities:
        output[--length] = keys[random() & 15];
    }
}
