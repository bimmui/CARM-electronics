/**************************************************************
 *
 *                     compression.h
 *
 *     Author(s):  Daniel Opara
 *     Date:       3/20/2024
 *
 *     Overview: Handles the compression of DLT values to 64-bit words
 *                  for each of the different states of the rocket
 *
 *
 **************************************************************/

#ifndef CARM_COMPRESSION_INCLUDED
#define CARM_COMPRESSION_INCLUDED

#include <inttypes.h>

uint64_t pack_poweron(unsigned int *transformed_data);

uint64_t *pack_launchready(unsigned int *transformed_data);

uint64_t *pack_launchmode(unsigned int *transformed_data);

uint64_t *pack_recovery(unsigned int *transformed_data);

#endif