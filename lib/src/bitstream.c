

#include "../include/bitstream.h"


size_t pushbits(BitStream* curr, unsigned char src, size_t nbit) {
    size_t bits_written = 0;

    while (nbit > 0) {
        size_t to_write = (nbit <= curr->capa) ? nbit : curr->capa;
        unsigned char aligned_bits = src >> (nbit - to_write);
        aligned_bits &= (1 << to_write) - 1;

        *curr->ptr |= aligned_bits << (curr->capa - to_write);
        curr->capa -= to_write;
        nbit -= to_write;
        bits_written += to_write;

        if (curr->capa == 0) {
            curr->ptr++;
            *curr->ptr = 0; // initialiser un nouveau octet
            curr->capa = CHAR_BIT;
        }
    }

    return bits_written;
}


size_t pullbits(BitStream* curr, unsigned char* dest, size_t nbit) {
    size_t bits_read = 0;
    *dest = 0;
    while (nbit > 0) {
        size_t to_read = (nbit <= curr->capa) ? nbit : curr->capa;
        unsigned char extracted_bits = (*curr->ptr >> (curr->capa - to_read)) & ((1 << to_read) - 1);
        *dest = (*dest << to_read) | extracted_bits;
        curr->capa -= to_read;
        nbit -= to_read;
        bits_read += to_read;
        if (curr->capa == 0) {
            curr->ptr++;
            curr->capa = CHAR_BIT;
        }
    }
    return bits_read;
}
