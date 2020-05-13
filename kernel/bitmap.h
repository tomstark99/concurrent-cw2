#ifndef __BITMAP_H
#define __BITMAP_H

#include <limits.h>    /* for CHAR_BIT */
#include <stdint.h>

typedef uint32_t word_t;

enum {
    BITS_PER_WORD = sizeof(word_t) * CHAR_BIT
};

#define WORD_OFFSET(b) ((b) / BITS_PER_WORD)
#define BIT_OFFSET(b)  ((b) % BITS_PER_WORD)

#endif
