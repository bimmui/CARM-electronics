/**************************************************************
 *
 *                     compression.c
 *
 *     Author(s):  Daniel Opara
 *     Date:       3/20/2024
 *
 *     Overview: Implementation of compression.h
 *
 *
 **************************************************************/

#include "compression.h"
#include "DLTransform.h"
#include <inttypes.h>

static const unsigned MAX_WORD_SIZE = 64;

/*
 * pack_poweron
 * Parameters: An array of unsigned ints that have gone through DLT
 * Returns: A single uint64_t
 * Purpose: Packs the transformed data into 64 bit word(s)
 * Notes: Follow guidelines of the POWER ON bitfield schema
 *
 */
uint64_t pack_poweron(unsigned int *transformed_data)
{
    uint64_t raw_word = 0;
    unsigned bit_count = MAX_WORD_SIZE;

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

/*
 * pack_launchready
 * Parameters: An array of unsigned ints that have gone through DLT
 * Returns: An array of uint64_t
 * Purpose: Packs the transformed data into 64 bit word(s)
 * Notes: Follow guidelines of the LAUNCH READY bitfield schema
 *
 */
uint64_t *pack_launchready(unsigned int *transformed_data)
{
    uint64_t raw_words[5] = {0, 0, 0, 0, 0};
    unsigned bit_count = MAX_WORD_SIZE;

    raw_words[0] = Bitpack_newu(raw_words[0], 4, bit_count -= 4,
                                transformed_data[0]);
    raw_words[0] = Bitpack_newu(raw_words[0], 3, bit_count -= 3,
                                transformed_data[1]);
    raw_words[0] = Bitpack_newu(raw_words[0], 1, bit_count -= 11,
                                transformed_data[2]);
    raw_words[0] = Bitpack_newu(raw_words[0], 28, bit_count -= 28,
                                transformed_data[3]);
    raw_words[0] = Bitpack_newu(raw_words[0], 1, bit_count -= 1,
                                transformed_data[4]);
    raw_words[0] = Bitpack_newu(raw_words[0], 27, bit_count -= 27,
                                transformed_data[5]);

    // reset bit_count for the next 64-bit word we're gonna pack
    bit_count = MAX_WORD_SIZE;
    raw_words[1] = Bitpack_newu(raw_words[1], 20, bit_count -= 20,
                                transformed_data[6]);
    raw_words[1] = Bitpack_newu(raw_words[1], 9, bit_count -= 9,
                                transformed_data[7]);
    raw_words[1] = Bitpack_newu(raw_words[1], 20, bit_count -= 20,
                                transformed_data[8]);
    raw_words[1] = Bitpack_newu(raw_words[1], 15, bit_count -= 15,
                                transformed_data[9]);

    bit_count = MAX_WORD_SIZE;
    raw_words[2] = Bitpack_newu(raw_words[2], 17, bit_count -= 17,
                                transformed_data[10]);
    raw_words[2] = Bitpack_newu(raw_words[2], 9, bit_count -= 9,
                                transformed_data[11]);
    raw_words[2] = Bitpack_newu(raw_words[2], 15, bit_count -= 15,
                                transformed_data[12]);
    raw_words[2] = Bitpack_newu(raw_words[2], 1, bit_count -= 1,
                                transformed_data[13]);
    raw_words[2] = Bitpack_newu(raw_words[2], 11, bit_count -= 11,
                                transformed_data[14]);
    raw_words[2] = Bitpack_newu(raw_words[2], 11, bit_count -= 11,
                                transformed_data[15]);

    bit_count = MAX_WORD_SIZE;
    raw_words[3] = Bitpack_newu(raw_words[3], 11, bit_count -= 11,
                                transformed_data[16]);
    raw_words[3] = Bitpack_newu(raw_words[3], 11, bit_count -= 11,
                                transformed_data[17]);
    raw_words[3] = Bitpack_newu(raw_words[3], 11, bit_count -= 11,
                                transformed_data[18]);
    raw_words[3] = Bitpack_newu(raw_words[3], 11, bit_count -= 11,
                                transformed_data[19]);
    raw_words[3] = Bitpack_newu(raw_words[3], 10, bit_count -= 10,
                                transformed_data[20]);
    raw_words[3] = Bitpack_newu(raw_words[3], 10, bit_count -= 10,
                                transformed_data[21]);

    bit_count = MAX_WORD_SIZE;
    raw_words[4] = Bitpack_newu(raw_words[4], 15, bit_count -= 15,
                                transformed_data[22]);
    raw_words[4] = Bitpack_newu(raw_words[4], 2, bit_count -= 2,
                                transformed_data[23]);
    raw_words[4] = Bitpack_newu(raw_words[4], 11, bit_count -= 11,
                                transformed_data[24]);
    raw_words[4] = Bitpack_newu(raw_words[4], 2, bit_count -= 2,
                                transformed_data[25]);

    return raw_words;
}

/*
 * pack_launchmode
 * Parameters: An array of unsigned ints that have gone through DLT
 * Returns: An array of uint64_t
 * Purpose: Packs the transformed data into 64 bit word(s)
 * Notes: Follow guidelines of the LAUNCH MODE bitfield schema
 *
 */
uint64_t *pack_launchmode(unsigned int *transformed_data)
{
    uint64_t raw_words[5] = {0, 0, 0, 0, 0};
    unsigned bit_count = MAX_WORD_SIZE;

    raw_words[0] = Bitpack_newu(raw_words[0], 4, bit_count -= 4,
                                transformed_data[0]);
    raw_words[0] = Bitpack_newu(raw_words[0], 3, bit_count -= 3,
                                transformed_data[1]);
    raw_words[0] = Bitpack_newu(raw_words[0], 1, bit_count -= 11,
                                transformed_data[2]);
    raw_words[0] = Bitpack_newu(raw_words[0], 28, bit_count -= 28,
                                transformed_data[3]);
    raw_words[0] = Bitpack_newu(raw_words[0], 1, bit_count -= 1,
                                transformed_data[4]);
    raw_words[0] = Bitpack_newu(raw_words[0], 27, bit_count -= 27,
                                transformed_data[5]);

    // reset bit_count for the next 64-bit word we're gonna pack
    bit_count = MAX_WORD_SIZE;
    raw_words[1] = Bitpack_newu(raw_words[1], 20, bit_count -= 20,
                                transformed_data[6]);
    raw_words[1] = Bitpack_newu(raw_words[1], 15, bit_count -= 15,
                                transformed_data[7]);
    raw_words[1] = Bitpack_newu(raw_words[1], 9, bit_count -= 9,
                                transformed_data[8]);
    raw_words[1] = Bitpack_newu(raw_words[1], 20, bit_count -= 20,
                                transformed_data[9]);

    bit_count = MAX_WORD_SIZE;
    raw_words[2] = Bitpack_newu(raw_words[2], 19, bit_count -= 19,
                                transformed_data[10]);
    raw_words[2] = Bitpack_newu(raw_words[2], 17, bit_count -= 17,
                                transformed_data[11]);
    raw_words[2] = Bitpack_newu(raw_words[2], 15, bit_count -= 15,
                                transformed_data[12]);
    raw_words[2] = Bitpack_newu(raw_words[2], 2, bit_count -= 2,
                                transformed_data[13]);
    raw_words[2] = Bitpack_newu(raw_words[2], 11, bit_count -= 11,
                                transformed_data[14]);

    bit_count = MAX_WORD_SIZE;
    raw_words[3] = Bitpack_newu(raw_words[3], 9, bit_count -= 9,
                                transformed_data[15]);
    raw_words[3] = Bitpack_newu(raw_words[3], 11, bit_count -= 11,
                                transformed_data[16]);
    raw_words[3] = Bitpack_newu(raw_words[3], 11, bit_count -= 11,
                                transformed_data[17]);
    raw_words[3] = Bitpack_newu(raw_words[3], 11, bit_count -= 11,
                                transformed_data[18]);
    raw_words[3] = Bitpack_newu(raw_words[3], 11, bit_count -= 11,
                                transformed_data[19]);
    raw_words[3] = Bitpack_newu(raw_words[3], 11, bit_count -= 11,
                                transformed_data[20]);

    bit_count = MAX_WORD_SIZE;
    raw_words[4] = Bitpack_newu(raw_words[4], 11, bit_count -= 11,
                                transformed_data[21]);
    raw_words[4] = Bitpack_newu(raw_words[4], 10, bit_count -= 10,
                                transformed_data[22]);
    raw_words[4] = Bitpack_newu(raw_words[4], 10, bit_count -= 10,
                                transformed_data[23]);
    raw_words[4] = Bitpack_newu(raw_words[4], 15, bit_count -= 15,
                                transformed_data[24]);
    raw_words[4] = Bitpack_newu(raw_words[4], 2, bit_count -= 2,
                                transformed_data[25]);
    raw_words[4] = Bitpack_newu(raw_words[4], 1, bit_count -= 1,
                                transformed_data[26]);

    return raw_words;
}

/*
 * pack_recovery
 * Parameters: An array of unsigned ints that have gone through DLT
 * Returns: An array of uint64_t
 * Purpose: Packs the transformed data into 64 bit word(s)
 * Notes: Follow guidelines of the RECOVERY MODE bitfield schema
 *
 */
uint64_t *pack_recovery(unsigned int *transformed_data)
{
    uint64_t raw_words[2] = {0, 0};
    unsigned bit_count = MAX_WORD_SIZE;

    raw_words[0] = Bitpack_newu(raw_words[0], 4, bit_count -= 4,
                                transformed_data[0]);
    raw_words[0] = Bitpack_newu(raw_words[0], 3, bit_count -= 3,
                                transformed_data[1]);
    raw_words[0] = Bitpack_newu(raw_words[0], 1, bit_count -= 11,
                                transformed_data[2]);
    raw_words[0] = Bitpack_newu(raw_words[0], 28, bit_count -= 28,
                                transformed_data[3]);
    raw_words[0] = Bitpack_newu(raw_words[0], 1, bit_count -= 1,
                                transformed_data[4]);
    raw_words[0] = Bitpack_newu(raw_words[0], 27, bit_count -= 27,
                                transformed_data[5]);

    // reset bit_count for the next 64-bit word we're gonna pack
    bit_count = MAX_WORD_SIZE;
    raw_words[1] = Bitpack_newu(raw_words[1], 11, bit_count -= 11,
                                transformed_data[6]);
    raw_words[1] = Bitpack_newu(raw_words[1], 11, bit_count -= 11,
                                transformed_data[7]);
    raw_words[1] = Bitpack_newu(raw_words[1], 11, bit_count -= 11,
                                transformed_data[8]);
    raw_words[1] = Bitpack_newu(raw_words[1], 1, bit_count -= 1,
                                transformed_data[9]);
    raw_words[1] = Bitpack_newu(raw_words[1], 10, bit_count -= 10,
                                transformed_data[10]);
    raw_words[1] = Bitpack_newu(raw_words[1], 2, bit_count -= 2,
                                transformed_data[11]);
    raw_words[1] = Bitpack_newu(raw_words[1], 2, bit_count -= 2,
                                transformed_data[12]);

    return raw_words;
}