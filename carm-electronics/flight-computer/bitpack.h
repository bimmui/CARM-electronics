#ifndef BITPACK_H
#define BITPACK_H

#include <inttypes.h>
#include <stdbool.h>

static uint64_t
shift_left(uint64_t word, unsigned bits);

static uint64_t
shift_right_logical(uint64_t word, unsigned bits);

static int64_t
shift_right_arith(uint64_t word, unsigned bits);

bool Bitpack_fitsu(uint64_t n, unsigned width);

bool Bitpack_fitss(int64_t n, unsigned width);

uint64_t
Bitpack_getu(uint64_t word, unsigned width, unsigned lsb);

int64_t
Bitpack_gets(uint64_t word, unsigned width, unsigned lsb);

uint64_t
Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value);

uint64_t
Bitpack_news(uint64_t word, unsigned width, unsigned lsb, int64_t value);

#endif