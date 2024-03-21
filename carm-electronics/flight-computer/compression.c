#include "compression.h"
#include "DLTransform.h"
#include <inttypes.h>

static const unsigned MAX_WORD_SIZE = 64;

uint64_t pack_poweron(unsigned int *transformed_data)
{
    uint64_t raw_word = 0;

    raw_word = Bitpack_newu(raw_word, 4, bit_count -= 4,
                            transformed_data[0]);
    raw_word = Bitpack_newu(raw_word, 11, bit_count -= 11,
                            transformed_data[1]);
    raw_word = Bitpack_newu(raw_word, 11, bit_count -= 11,
                            transformed_data[2]);
    raw_word = Bitpack_newu(raw_word, 11, bit_count -= 11,
                            transformed_data[3]);
    raw_word = Bitpack_newu(raw_word, 2, bit_count -= 2,
                            transformed_data[4]);
    raw_word = Bitpack_newu(raw_word, 1, bit_count -= 1,
                            transformed_data[5]);
    raw_word = Bitpack_newu(raw_word, 3, bit_count -= 3,
                            transformed_data[6]);
    raw_word = Bitpack_newu(raw_word, 2, bit_count -= 2,
                            transformed_data[7]);
    raw_word = Bitpack_newu(raw_word, 10, bit_count -= 10,
                            transformed_data[8]);

    return raw_word;
}