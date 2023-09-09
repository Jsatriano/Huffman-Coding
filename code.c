#include "defines.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Code {
    uint32_t top;
    uint8_t bits[MAX_CODE_SIZE];
} Code;

uint8_t code_getting_bit(Code *c, uint32_t index) { //helper function that gets a bit in a code
    uint8_t result = c->bits[index / 8] & (1 << (index % 8));
    uint8_t ans = result >> (index % 8);
    return ans;
}

void code_clr_bit(Code *c, uint32_t index) { //helper function that clears a bit in a code
    c->bits[index / 8] &= ~(1 << (index % 8));
}

Code code_init(void) {
    Code c;
    c.top = 0;
    return c;
}

uint32_t code_size(Code *c) {
    return c->top;
}

bool code_empty(Code *c) {
    return c->top == 0;
}

bool code_full(Code *c) {
    return c->top == MAX_CODE_SIZE;
}

bool code_push_bit(Code *c, uint8_t bit) {
    if (code_full(c)) {
        return false;
    } else {
        c->bits[c->top / 8] |= (bit << (c->top % 8));
        c->top += 1;
        return true;
    }
}

bool code_pop_bit(Code *c, uint8_t *bit) {
    if (code_empty(c)) {
        return false;
    } else {
        c->top -= 1;
        *bit = code_getting_bit(c, c->top);
        code_clr_bit(c, c->top);
        return true;
    }
}

void code_print(Code *c) {
    printf("[");
    for (uint32_t i = 0; i < c->top; i += 1) {
        printf("%u", c->bits[i]);
        if (i + 1 != c->top) {
            printf(" ");
        }
    }
    printf("]\n");
}
