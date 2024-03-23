#include <stdbool.h>
#include <inttypes.h>
#include "bitpack.h"
#include "assert.h"

// Except_T Bitpack_Overflow = { "Overflow packing bits" };

static const unsigned MAX_BITS = 64;

/**************************** shift_left() ************************
 *
 *  Purpose: Shifts the bits of an unsigned 64-bit integer to the left by a
 *           specified number of bits.
 *
 *  Parameters:
 *    1. word -- an unsigned 64-bit integer representing the binary word to be
 *               shifted
 *    2. bits -- an unsigned integer representing the number of bits by which to
 *              shift the word.
 *
 *  Returns: An unsigned 64-bit integer representing the word shifted to the
 *           left by the specified number of bits.
 *
 *  Effects: It is a Checked Runtime Error if the number of bits to be
 *           shifted exceeds the maximum number of bytes (8 bytes or 64 bits).
 *           If the number of bits to shift exceeds the maximum number of bytes,
 *           the function returns 0.
 *
 *  Expects: The word parameter should be a valid 64-bit integer value.
 *
 ****************************************************************************/
static uint64_t shift_left(uint64_t word, unsigned bits)
{
    assert(bits <= MAX_BITS);
    if (bits == MAX_BITS)
    {
        return 0;
    }
    else
    {
        return word << bits;
    }
}

/************************** shift_right_logical() ************************
 *
 *  Purpose: Performs a logical right shift on an input word by a specified
 *           number of bits.
 *
 *  Parameters:
 *     1. word -- a 64-bit unsigned integer representing the input word to be
 *                shifted.
 *    2. bits -- an unsigned integer representing the number of bits by which to
 *               shift the input word.
 *
 *  Returns: A 64-bit unsigned integer representing the result of the logical
 *           right shift operation on the input word.
 *
 *  Effects: It is a Checked Runtime Error if the number of bits to be
 *           shifted exceeds the maximum number of bytes (8 bytes or 64 bits).
 *           If the number of bits to shift exceeds the maximum number of bytes,
 *           the function returns 0.
 *
 *  Expects: This function expects the input word to be a 64-bit unsigned
 *           integer. Also, the word parameter must be a valid
 *           64-bit unsigned integer.
 *
 ****************************************************************************/
static uint64_t shift_right_logical(uint64_t word, unsigned bits)
{
    assert(bits <= MAX_BITS);
    if (bits == MAX_BITS)
    {
        return 0;
    }
    else
    {
        return word >> bits;
    }
}

/************************** shift_right_arith () ************************
 *
 *  Purpose: Performs an arithmetic right shift on a 64-bit word by a specified
 *           number of bits.
 *
 *  Parameters:
 *      1. word -- a 64-bit unsigned integer that represents the word to be
 *                 shifted
 *      2. bits --  an unsigned integer that represents the number of bits to
 *                  shift the word by
 *
 *  Returns: An int64_t value that represents the result of the arithmetic
 *           right shift
 *
 *  Effects: It is a Checked Runtime Error if the number of bits to be shifted
 *           exceeds the maximum number of bytes (8 bytes or 64 bits).
 *           If bits is equal to MAX_BITS, the function sets bits to 63.
 *
 *  Expects: The bits parameter must be less than or equal to the maximum
 *           number of bytes (MAX_BITS) allowed in the program. Also, the word
 *           parameter must be a valid 64-bit unsigned integer.
 *
 ****************************************************************************/
static int64_t shift_right_arith(uint64_t word, unsigned bits)
{
    assert(bits <= MAX_BITS);
    if (bits == MAX_BITS)
    {
        bits = 31;
    }
    return ((int64_t)word) >> bits;
}

/**************************** Bitpack_fitsu() ************************
 *
 *  Purpose:  Determines if an unsigned integer value can fit into a given bit
 *            width without overflow
 *
 *  Parameters:
 *      1. n -- An unsigned 64-bit integer value to check if it can fit into the
 *             given bit width.
 *      2. width -- An unsigned integer value representing the bit width to
 *                  check if the given value can fit into.
 *
 *  Returns: A boolean value indicating whether the given value can fit into the
 *           given bit width without overflow.
 *
 *  Effects: The function has no side effects on external variables or state.
 *
 *  Expects: The input parameters must be valid and within their respective
 *           allowable ranges. In particular, it is a Checked Runtime Error if
 *           the width value exceeds MAX_BITS, the maximum allowable number of
 *           bytes, (8 bytes or 64 bits)
 *
 ****************************************************************************/
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
    assert(width <= MAX_BITS);
    uint64_t result = shift_right_logical(n, width);
    return result == 0;
}

/**************************** Bitpack_fitss() ************************
 *
 *  Purpose: The purpose of the function Bitpack_fitss is to determine whether
 *           an signed integer value can be represented using a given number of
 *           bits width using two's complement encoding.
 *
 *  Parameters:
 *      1. n -- An signed integer value to check if it can fit into the
 *             given bit width.
 *      2. width -- An unsigned integer indicating the number of bits available
 *                  for encoding n in two's complement.
 *
 *  Returns: The function returns a bool value that indicates whether the
 *           int64_t value n can be represented using the given number of bits
 *           width using two's complement encoding. The function returns true
 *           if n can be represented and false otherwise.
 *
 *  Effects: It is a Checked Runtime Error if width exceeds MAX_BITS, the
 *           maximum allowable number of bytes, (8 bytes or 64 bits)
 *
 *  Expects:  The function expects that the parameter width is less than or
 *            equal to MAX_BITS.
 *
 ****************************************************************************/
bool Bitpack_fitss(int64_t n, unsigned width)
{
    assert(width <= MAX_BITS);
    uint64_t shifted_left = shift_left(n, MAX_BITS - width);
    int64_t result = shift_right_arith(shifted_left, MAX_BITS - width);
    return result == n;
}

/**************************** Bitpack_getu() ************************
 *
 *  Purpose: Extracts an unsigned integer of a specified width from a given
 *           word, starting from a specified least significant bit (lsb).
 *
 *  Parameters:
 *      1. word -- an unsigned 64-bit integer from which bits will be extracted
 *      2. width -- an unsigned integer representing the width (in bits) of the
 *                  unsigned integer to be extracted.
 *      2. lsb -- an unsigned integer representing the index of the least
 *                significant bit of the unsigned integer to be extracted
 *
 *  Returns: An unsigned 64-bit integer representing the extracted unsigned
 *           integer.
 *
 *  Effects: It is a Checked Runtime Error if width or the sum of width and lsb
 *           exceeds MAX_BITS, the maximum allowable number of bytes,
 *           (8 bytes or 64 bits)
 *
 *  Expects: Assumes that width and lsb are both less than or equal to
 *           MAX_BITS. The sum of width and lsb is less than or equal to
 *           MAX_BITS.
 *
 ****************************************************************************/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width <= MAX_BITS);
    unsigned shift_diff = lsb + width;
    assert(shift_diff <= MAX_BITS);
    uint64_t aligned = shift_left(word, (MAX_BITS - (lsb + width)));
    uint64_t extracted = shift_right_logical(aligned, (MAX_BITS - width));
    return extracted;
}

/**************************** Bitpack_gets() ************************
 *
 *  Purpose: Extracts a signed integer of a specified width from a given
 *           word, starting from a specified least significant bit (lsb).
 *
 *  Parameters:
 *      1. word -- the bit-packed word from which to extract the signed value
 *      2. width -- the number of bits to extract
 *      3. lsb --  the index of the least significant bit of the value to be
 *                 extracted in the word
 *
 *  Returns: An signed 64-bit integer representing the extracted signed
 *           integer.
 *
 *  Effects: It is a Checked Runtime Error if width or the sum of width and lsb
 *           exceeds MAX_BITS, the maximum allowable number of bytes,
 *           (8 bytes or 64 bits)
 *
 *  Expects: Assumes that width and lsb are both less than or equal to
 *           MAX_BITS. The sum of width and lsb is less than or equal to
 *           MAX_BITS.
 *
 ****************************************************************************/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width <= MAX_BITS);
    assert(lsb + width <= MAX_BITS);
    if (width == 0)
    {
        return 0;
    }
    uint64_t aligned = shift_left(word, (MAX_BITS - (lsb + width)));
    int64_t extracted = shift_right_arith(aligned, (MAX_BITS - width));
    return extracted;
}

/**************************** Bitpack_newu() ************************
 *
 *  Purpose:  Packs a new unsigned value of a specified width into a given
 *            word at a specified least significant bit position.
 *
 *  Parameters:
 *      1. word --  a 64-bit unsigned integer representing the word to be
 *                  modified
 *      2. width --- an unsigned integer representing the width of the field to
 *                   be modified
 *      3. lsb -- an unsigned integer representing the least significant bit
 *                position of the field to be modified
 *      4. value -- a 64-bit unsigned integer representing the new value to be
 *                 packed into the field
 *
 *  Returns: A 64-bit unsigned integer representing the modified word with the
 *           new value packed in the specified field.
 *
 *  Effects:  It is a Checked Runtime Error if width or the sum of width and lsb
 *           exceeds MAX_BITS, the maximum allowable number of bytes,
 *           (8 bytes or 64 bits). The function will throw an exception
 *           Bitpack_Overflow if given a value that does not fit in width bits.
 *
 *  Expects: Assumes that width and lsb are both less than or equal to
 *           MAX_BITS. The sum of width and lsb is less than or equal to
 *           MAX_BITS.
 ****************************************************************************/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                      uint64_t value)
{
    unsigned shift_high = lsb + width;
    unsigned shift_low = MAX_BITS - lsb;
    assert(shift_high <= 64);
    assert(width <= 64);
    if (!(Bitpack_fitsu(value, width)))
    {
        // RAISE(Bitpack_Overflow);
    }
    /*  right-shifts the word by the index of the MSB to align the bits
    of the field with the most significant bits of the word */
    uint64_t align_high = shift_right_logical(word, shift_high);
    /*  left-shifts the aligned bits to the leftmost bits of the word, so
    that they are ready to be combined with the value */
    uint64_t high_end = shift_left(align_high, shift_high);
    /* left-shifts the bits that are not included in the field being written
    to the leftmost bits of the word, so that they are ready to be combined
    with the value */
    uint64_t align_low = shift_left(word, shift_low);
    /* right-shifts the left-shifted bits to align them with the LSB's of
    the word */
    uint64_t low_end = shift_right_logical(align_low, shift_low);
    /* combines the aligned bits of the field with the most significant
    bits of the word (high_end), the value to be written, left-shifted by
    the least significant bit (value << lsb), and the aligned bits of the
    field with the least significant bits */
    uint64_t results = high_end | (value << lsb) | low_end;

    return results;
}

/**************************** Bitpack_news() ************************
 *
 *  Purpose:  Packs a new signed value of a specified width into a given
 *            word at a specified least significant bit position.
 *
 *  Parameters:
 *      1. word -- a uint64_t representing the original word to be modified
 *      2. width --  an unsigned integer representing the number of bits in the
 *                   field to be modified
 *      3. lsb -- an unsigned integer representing the index of the least
 *                significant bit of the field to be modified
 *      4. value -- an int64_t representing the value to be inserted into the
 *                  specified field, using sign extension if necessary
 *
 *
 *  Returns: A uint64_t representing the modified word with the new value
 *           inserted into the specified field
 *
 *  Effects: It is a Checked Runtime Error if width or the sum of width and lsb
 *           exceeds MAX_BITS, the maximum allowable number of bytes,
 *           (8 bytes or 64 bits). The function will throw an exception
 *           Bitpack_Overflow if given a value that does not fit in width bits.
 *
 *  Expects: Assumes that width and lsb are both less than or equal to
 *           MAX_BITS. The sum of width and lsb is less than or equal to
 *           MAX_BITS.
 *
 ****************************************************************************/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
                      int64_t value)
{
    assert(width <= 64);
    assert(width + lsb <= 64);
    if (!(Bitpack_fitss(value, width)))
    {
        // RAISE(Bitpack_Overflow);
    }
    uint64_t extracted_value = Bitpack_getu(value, width, 0);
    return Bitpack_newu(word, width, lsb, extracted_value);
}