#include "decompression.h"
#include "bitpack.h"
#include <inttypes.h>

static const unsigned MAX_WORD_SIZE = 64;

// TODO: we should just return a struct
unsigned int *unpack_poweron(uint64_t poweron_data)
{
    unsigned bit_count = MAX_WORD_SIZE;
    unsigned int unpacked_values[8];

    unpacked_values[0] = Bitpack_getu(poweron_data, 4, bit_count -= 4);
    unpacked_values[1] = Bitpack_getu(poweron_data, 11, bit_count -= 11);
    unpacked_values[2] = Bitpack_getu(poweron_data, 11, bit_count -= 11);
    unpacked_values[3] = Bitpack_getu(poweron_data, 11, bit_count -= 11);
    unpacked_values[4] = Bitpack_getu(poweron_data, 2, bit_count -= 2);
    unpacked_values[5] = Bitpack_getu(poweron_data, 1, bit_count -= 1);
    unpacked_values[6] = Bitpack_getu(poweron_data, 3, bit_count -= 3);
    unpacked_values[7] = Bitpack_getu(poweron_data, 2, bit_count -= 2);
    unpacked_values[8] = Bitpack_getu(poweron_data, 10, bit_count -= 10);

    return unpacked_values;
}