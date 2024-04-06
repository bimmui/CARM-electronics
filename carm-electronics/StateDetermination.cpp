/**************************************************************
 *
 *                     StateDetermination.cpp
 *
 *     Author(s):  Daniel Opara
 *     Date:       3/20/2024
 *
 *     Overview: Implementation of StateDetermination.h
 *
 *
 **************************************************************/

#include "StateDetermination.h"
#include "BBManager.h"
#include "bitpack.h"

// TODO: Complete this func, takes in bbman and modifies its curr_state attribute
void state_determiner(BBManager manager)
{
}

state get_state_from_bits(uint64_t packet)
{
    state extracted_state = Bitpack_getu(packet, 4, 60);
    return extracted_state;
}