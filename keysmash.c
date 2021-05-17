#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char* keysmash(unsigned length) {
    char keys[16] = { 'a', 's', 'd', 'f', 'g', 'j', 'k', 'l', 'a', 's', 'd', 'f', 'j', 'k', 'l', ';' };
    // Special case for this, otherwise we're writing out of bounds!
    if (length == 1) {
        return "a";
    }
    char* smash = (char*)malloc(sizeof(char)*(length+1));
    smash[0] = 'a';
    smash[1] = 's';
    smash[length] = '\0';
    smash += 2;
    length -= 2;
    /* 0 is probably fewer cycles to check against */
    while (length != 0) {
        // random() is a 32 bit int. we have 16 possibilities:
        smash[--length] = keys[random() & 15];
    }
    smash -= 2;
    return smash;
}
