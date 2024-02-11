#ifndef BITPACK_H
#define BITPACK_H

static uint32_t
shift_left(uint32_t word, unsigned bits);

static uint32_t
shift_right_logical(uint32_t word, unsigned bits);

static int32_t
shift_right_arith(uint32_t word, unsigned bits);

bool Bitpack_fitsu(uint32_t n, unsigned width);

bool Bitpack_fitss(int32_t n, unsigned width);

uint32_t
Bitpack_getu(uint32_t word, unsigned width, unsigned lsb);

uint32_t
Bitpack_newu(uint32_t word, unsigned width, unsigned lsb, uint32_t value);

#endif