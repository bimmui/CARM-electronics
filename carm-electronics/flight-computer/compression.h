#include "bitpack.h"

static const unsigned MAX_WORD_SIZE = 64;

uint64_t packPowerOn(BBManager bbman)
{
    uint64_t raw_word = 0;
    unsigned bit_count = MAX_WORD_SIZE;

    // TODO: the actual values that would be packed are where bbman is
    // there needs to be some math done to them before compressing them to bits
    raw_word = Bitpack_newu(raw_word, 4, bit_count -= 4,
                            bbman.state);
    raw_word = Bitpack_newu(raw_word, 11, bit_count -= 11,
                            bbman.ext_temp);
    raw_word = Bitpack_newu(raw_word, 11, bit_count -= 11,
                            bbman.int_temp_eng);
    raw_word = Bitpack_newu(raw_word, 11, bit_count -= 11,
                            bbman.int_temp_avb);
    raw_word = Bitpack_newu(raw_word, 2, bit_count -= 2,
                            bbman.gps_sigq);
    raw_word = Bitpack_newu(raw_word, 1, bit_count -= 1,
                            bbman.gps_fix);
    raw_word = Bitpack_newu(raw_word, 3, bit_count -= 3,
                            bbman.gps_link);
    raw_word = Bitpack_newu(raw_word, 2, bit_count -= 2,
                            bbman.gps_antenna);
    raw_word = Bitpack_newu(raw_word, 10, bit_count -= 10,
                            bbman.gps_antenna);

    return raw_word;
}