/**************************************************************
 *
 *                     decompression.c
 *
 *     Author(s):  Daniel Opara
 *     Date:       3/20/2024
 *
 *     Overview: Implementation of decompression.h
 *
 *
 **************************************************************/

#include "decompression.h"
#include "bitpack.h"
#include <inttypes.h>

static const unsigned MAX_WORD_SIZE = 64;

/*
 * unpack_poweron
 * Parameters: A single uint64
 * Returns: An array of unsigned ints
 * Purpose: Unpacks the 64 bit word(s) into its DLT values
 * Notes: Follow guidelines of the POWER ON bitfield schema
 *
 */
unsigned int *unpack_poweron(uint64_t poweron_data)
{
    unsigned bit_count = MAX_WORD_SIZE;
    static unsigned int unpacked_values[8];

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

/*
 * unpack_launchready
 * Parameters: A single uint64
 * Returns: An array of unsigned ints
 * Purpose: Unpacks the 64 bit word(s) into its DLT values
 * Notes: Follow guidelines of the LAUNCH READY bitfield schema
 *
 */
unsigned int *unpack_launchready(uint64_t *launchready_data)
{
    unsigned bit_count = MAX_WORD_SIZE;
    static unsigned int unpacked_values[26];

    //! TODO: Test to ensure that receiving a transmitted data returns an array of values
    //              as we expect. If not, everything below is prob wrong
    // word 1
    unpacked_values[0] = Bitpack_getu(launchready_data[0], 4, bit_count -= 4);
    unpacked_values[1] = Bitpack_getu(launchready_data[0], 3, bit_count -= 3);
    unpacked_values[2] = Bitpack_getu(launchready_data[0], 1, bit_count -= 1);
    unpacked_values[3] = Bitpack_getu(launchready_data[0], 28, bit_count -= 28);
    unpacked_values[4] = Bitpack_getu(launchready_data[0], 1, bit_count -= 1);
    unpacked_values[5] = Bitpack_getu(launchready_data[0], 27, bit_count -= 27);

    // word 2
    bit_count = MAX_WORD_SIZE;
    unpacked_values[6] = Bitpack_getu(launchready_data[1], 20, bit_count -= 20);
    unpacked_values[7] = Bitpack_getu(launchready_data[1], 9, bit_count -= 9);
    unpacked_values[8] = Bitpack_getu(launchready_data[1], 20, bit_count -= 20);
    unpacked_values[9] = Bitpack_getu(launchready_data[1], 15, bit_count -= 15);

    // word 3
    bit_count = MAX_WORD_SIZE;
    unpacked_values[10] = Bitpack_getu(launchready_data[2], 17, bit_count -= 17);
    unpacked_values[11] = Bitpack_getu(launchready_data[2], 9, bit_count -= 9);
    unpacked_values[12] = Bitpack_getu(launchready_data[2], 15, bit_count -= 15);
    unpacked_values[13] = Bitpack_getu(launchready_data[2], 1, bit_count -= 1);
    unpacked_values[14] = Bitpack_getu(launchready_data[2], 11, bit_count -= 11);
    unpacked_values[15] = Bitpack_getu(launchready_data[2], 11, bit_count -= 11);

    // word 4
    bit_count = MAX_WORD_SIZE;
    unpacked_values[16] = Bitpack_getu(launchready_data[3], 11, bit_count -= 11);
    unpacked_values[17] = Bitpack_getu(launchready_data[3], 11, bit_count -= 11);
    unpacked_values[18] = Bitpack_getu(launchready_data[3], 11, bit_count -= 11);
    unpacked_values[19] = Bitpack_getu(launchready_data[3], 11, bit_count -= 11);
    unpacked_values[20] = Bitpack_getu(launchready_data[3], 10, bit_count -= 10);
    unpacked_values[21] = Bitpack_getu(launchready_data[3], 10, bit_count -= 10);

    // word 5
    bit_count = MAX_WORD_SIZE;
    unpacked_values[22] = Bitpack_getu(launchready_data[4], 15, bit_count -= 15);
    unpacked_values[23] = Bitpack_getu(launchready_data[4], 2, bit_count -= 2);
    unpacked_values[24] = Bitpack_getu(launchready_data[4], 11, bit_count -= 11);
    unpacked_values[25] = Bitpack_getu(launchready_data[4], 2, bit_count -= 2);

    return unpacked_values;
}

/*
 * unpack_launchmode
 * Parameters: A single uint64
 * Returns: An array of unsigned ints
 * Purpose: Unpacks the 64 bit word(s) into its DLT values
 * Notes: Follow guidelines of the LAUNCH READY bitfield schema
 *
 */
unsigned int *unpack_launchmode(uint64_t *launchmode_data)
{
    unsigned bit_count = MAX_WORD_SIZE;
    static unsigned int unpacked_values[27];

    //! TODO: Test to ensure that receiving a transmitted data returns an array of values
    //              as we expect. If not, everything below is prob wrong
    // word 1
    unpacked_values[0] = Bitpack_getu(launchmode_data[0], 4, bit_count -= 4);
    unpacked_values[1] = Bitpack_getu(launchmode_data[0], 3, bit_count -= 3);
    unpacked_values[2] = Bitpack_getu(launchmode_data[0], 1, bit_count -= 1);
    unpacked_values[3] = Bitpack_getu(launchmode_data[0], 28, bit_count -= 28);
    unpacked_values[4] = Bitpack_getu(launchmode_data[0], 1, bit_count -= 1);
    unpacked_values[5] = Bitpack_getu(launchmode_data[0], 27, bit_count -= 27);

    // word 2
    bit_count = MAX_WORD_SIZE;
    unpacked_values[6] = Bitpack_getu(launchmode_data[1], 20, bit_count -= 20);
    unpacked_values[7] = Bitpack_getu(launchmode_data[1], 15, bit_count -= 15);
    unpacked_values[8] = Bitpack_getu(launchmode_data[1], 9, bit_count -= 9);
    unpacked_values[9] = Bitpack_getu(launchmode_data[1], 20, bit_count -= 20);

    // word 3
    bit_count = MAX_WORD_SIZE;
    unpacked_values[10] = Bitpack_getu(launchmode_data[2], 19, bit_count -= 19);
    unpacked_values[11] = Bitpack_getu(launchmode_data[2], 17, bit_count -= 17);
    unpacked_values[12] = Bitpack_getu(launchmode_data[2], 15, bit_count -= 15);
    unpacked_values[13] = Bitpack_getu(launchmode_data[2], 2, bit_count -= 2);
    unpacked_values[14] = Bitpack_getu(launchmode_data[2], 11, bit_count -= 11);

    // word 4
    bit_count = MAX_WORD_SIZE;
    unpacked_values[15] = Bitpack_getu(launchmode_data[3], 9, bit_count -= 9);
    unpacked_values[16] = Bitpack_getu(launchmode_data[3], 11, bit_count -= 11);
    unpacked_values[17] = Bitpack_getu(launchmode_data[3], 11, bit_count -= 11);
    unpacked_values[18] = Bitpack_getu(launchmode_data[3], 11, bit_count -= 11);
    unpacked_values[19] = Bitpack_getu(launchmode_data[3], 11, bit_count -= 11);
    unpacked_values[20] = Bitpack_getu(launchmode_data[3], 11, bit_count -= 11);

    // word 5
    bit_count = MAX_WORD_SIZE;
    unpacked_values[21] = Bitpack_getu(launchmode_data[4], 11, bit_count -= 11);
    unpacked_values[22] = Bitpack_getu(launchmode_data[4], 10, bit_count -= 10);
    unpacked_values[23] = Bitpack_getu(launchmode_data[4], 10, bit_count -= 10);
    unpacked_values[24] = Bitpack_getu(launchmode_data[4], 15, bit_count -= 15);
    unpacked_values[25] = Bitpack_getu(launchmode_data[4], 2, bit_count -= 2);
    unpacked_values[26] = Bitpack_getu(launchmode_data[4], 1, bit_count -= 1);

    return unpacked_values;
}

/*
 * unpack_poweron
 * Parameters: A single uint64
 * Returns: An array of unsigned ints
 * Purpose: Unpacks the 64 bit word(s) into its DLT values
 * Notes: Follow guidelines of the POWER ON bitfield schema
 *
 */
unsigned int *unpack_recovery(uint64_t *recovery_data)
{
    unsigned bit_count = MAX_WORD_SIZE;
    static unsigned int unpacked_values[13];

    // word 1
    unpacked_values[0] = Bitpack_getu(recovery_data[0], 4, bit_count -= 4);
    unpacked_values[1] = Bitpack_getu(recovery_data[0], 3, bit_count -= 3);
    unpacked_values[2] = Bitpack_getu(recovery_data[0], 1, bit_count -= 1);
    unpacked_values[3] = Bitpack_getu(recovery_data[0], 28, bit_count -= 28);
    unpacked_values[4] = Bitpack_getu(recovery_data[0], 1, bit_count -= 1);
    unpacked_values[5] = Bitpack_getu(recovery_data[0], 27, bit_count -= 27);

    // word 2
    bit_count = MAX_WORD_SIZE;
    unpacked_values[6] = Bitpack_getu(recovery_data[1], 11, bit_count -= 11);
    unpacked_values[7] = Bitpack_getu(recovery_data[1], 11, bit_count -= 11);
    unpacked_values[8] = Bitpack_getu(recovery_data[1], 11, bit_count -= 11);
    unpacked_values[9] = Bitpack_getu(recovery_data[1], 1, bit_count -= 1);
    unpacked_values[10] = Bitpack_getu(recovery_data[1], 10, bit_count -= 10);
    unpacked_values[11] = Bitpack_getu(recovery_data[1], 2, bit_count -= 2);
    unpacked_values[12] = Bitpack_getu(recovery_data[1], 2, bit_count -= 2);

    return unpacked_values;
}

unsigned int *unpack_noschema(uint64_t *data)
{
    unsigned bit_count = MAX_WORD_SIZE;
    static unsigned int unpacked_values[27];

    //! TODO: Test to ensure that receiving a transmitted data returns an array of values
    //              as we expect. If not, everything below is prob wrong
    // word 1
    unpacked_values[0] = Bitpack_getu(data[0], 4, bit_count -= 4);
    unpacked_values[1] = Bitpack_getu(data[0], 3, bit_count -= 3);
    unpacked_values[2] = Bitpack_getu(data[0], 1, bit_count -= 1);
    unpacked_values[3] = Bitpack_getu(data[0], 28, bit_count -= 28);
    unpacked_values[4] = Bitpack_getu(data[0], 1, bit_count -= 1);
    unpacked_values[5] = Bitpack_getu(data[0], 27, bit_count -= 27);

    // word 2
    bit_count = MAX_WORD_SIZE;
    unpacked_values[10] = Bitpack_getu(data[1], 25, bit_count -= 25);
    unpacked_values[6] = Bitpack_getu(data[1], 20, bit_count -= 20);
    unpacked_values[22] = Bitpack_getu(data[1], 10, bit_count -= 10);
    unpacked_values[15] = Bitpack_getu(data[1], 9, bit_count -= 9);

    // word 3
    bit_count = MAX_WORD_SIZE;
    unpacked_values[9] = Bitpack_getu(data[2], 20, bit_count -= 20);
    unpacked_values[11] = Bitpack_getu(data[2], 17, bit_count -= 17);
    unpacked_values[7] = Bitpack_getu(data[2], 15, bit_count -= 15);
    unpacked_values[23] = Bitpack_getu(data[2], 10, bit_count -= 10);
    unpacked_values[13] = Bitpack_getu(data[2], 2, bit_count -= 2);

    // word 4
    bit_count = MAX_WORD_SIZE;
    unpacked_values[21] = Bitpack_getu(data[3], 11, bit_count -= 11);
    unpacked_values[12] = Bitpack_getu(data[3], 15, bit_count -= 15);
    unpacked_values[24] = Bitpack_getu(data[3], 15, bit_count -= 15);
    unpacked_values[14] = Bitpack_getu(data[3], 11, bit_count -= 11);
    unpacked_values[16] = Bitpack_getu(data[3], 11, bit_count -= 11);
    unpacked_values[26] = Bitpack_getu(data[3], 1, bit_count -= 1);

    // word 5
    bit_count = MAX_WORD_SIZE;
    unpacked_values[17] = Bitpack_getu(data[4], 11, bit_count -= 11);
    unpacked_values[18] = Bitpack_getu(data[4], 11, bit_count -= 11);
    unpacked_values[20] = Bitpack_getu(data[4], 11, bit_count -= 11);
    unpacked_values[8] = Bitpack_getu(data[4], 9, bit_count -= 9);
    unpacked_values[19] = Bitpack_getu(data[4], 11, bit_count -= 11);
    unpacked_values[25] = Bitpack_getu(data[4], 2, bit_count -= 2);

    return unpacked_values;
}