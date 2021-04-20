#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* keysmash(int length) {
    char keys[] = { 'a', 's', 'd', 'f', 'j', 'k', 'l', 'a', 's', 'd', 'f', 'j', 'k', 'l', ';' };
    length -= 2;
    char* smash = (char*)malloc(sizeof(char)*1000);
    smash[0] = 'a';
    smash[1] = 's';
    smash[2] = '\0';
    while (length > 0) {
        length -= 1;
        strncat(smash, &keys[rand() % 15], 1);
    }
    return smash;
}
