#include "io.h"

#include "code.h"
#include "defines.h"

#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static uint32_t bit_index = 0;
static uint8_t buffer[BLOCK];

void set_bit(uint8_t *buf, uint32_t index) { //helper function that sets a bit in a code
    buf[index / 8] |= (1 << (index % 8));
}

void clr_bit(uint8_t *buf, uint32_t index) { //helper function that clears a bit in a code
    buf[index / 8] &= ~(1 << (index % 8));
}

uint8_t code_get_bit(Code *c, uint32_t index) { //helper function that gets a bit in a code
    uint8_t result = c->bits[index / 8] & (1 << (index % 8));
    uint8_t ans = result >> (index % 8);
    return ans;
}

uint8_t get_bit(uint8_t *buf, uint32_t index) { //helper function that gets a bit in a buffer
    uint8_t result = buf[index / 8] & (1 << (index % 8));
    uint8_t ans = result >> (index % 8);
    return ans;
}

int read_bytes(int infile, uint8_t *buf, int nbytes) {
    int total_bytes = 0;
    int read_bytes = 1;
    while (read_bytes > 0 && total_bytes != nbytes) {
        read_bytes = read(infile, buf, nbytes - total_bytes);
        total_bytes += read_bytes;
    }
    return total_bytes;
}

int write_bytes(int outfile, uint8_t *buf, int nbytes) {
    int total_bytes = 0;
    int read_bytes = 1;
    while (read_bytes > 0 && total_bytes != nbytes) {
        read_bytes = write(outfile, buf, nbytes - total_bytes);
        bytes_written += nbytes - total_bytes; //stat_counter
        total_bytes += read_bytes;
    }

    return total_bytes;
}

bool read_bit(int infile, uint8_t *bit) {
    bytes_read += 1; //stat counter
    uint32_t final_bit = 0;
    if (bit_index == 0) {
        uint32_t bytes_read = read_bytes(infile, buffer, BLOCK);
        if (bytes_read < BLOCK) {
            final_bit = (bytes_read * 8) + 1;
        }
    }
    *bit = get_bit(buffer, bit_index);
    bit_index += 1;
    if (bit_index == (8 * BLOCK)) {
        bit_index = 0;
    }

    if (bit_index == final_bit) {
        return false;
    } else {
        return true;
    }
}

void write_code(int outfile, Code *c) {
    for (uint32_t i = 0; i < code_size(c); i += 1) {
        if (code_get_bit(c, i) == 1) {
            set_bit(buffer, bit_index);
        } else {
            clr_bit(buffer, bit_index);
        }
        bit_index += 1;

        if (bit_index == (8 * BLOCK)) {
            write_bytes(outfile, buffer, BLOCK);
            bit_index = 0;
        }
    }
}

void flush_codes(int outfile) {
    if (bit_index > 0) {
        int remaining = 0;
        if (bit_index % 8 == 0) {
            remaining = bit_index / 8;
        } else {
            remaining = (bit_index / 8) + 1;
        }
        write_bytes(outfile, buffer, remaining);
    }
}
