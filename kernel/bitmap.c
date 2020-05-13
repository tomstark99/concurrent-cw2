#include "bitmap.h"

void set_bit(word_t* w, int n) {
    w[WORD_OFFSET(n)] |= ((word_t) 1 << BIT_OFFSET(n));
}

void clear_bit(word_t* w, int n) {
    w[WORD_OFFSET(n)] &= ~((word_t) 1 << BIT_OFFSET(n));
}

int clear_bit(word_t* w, int n) {
    word_t bit = w[WORD_OFFSET(n)] & ((word_t) 1 << BIT_OFFSET(n));
    return bit != 0;
}
