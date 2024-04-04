/**************************************************************
 *
 *                     decompression.h
 *
 *     Author(s):  Daniel Opara
 *     Date:       3/20/2024
 *
 *     Overview: Handles the decompression of 64-bit words to DLT values
 *                  for each of the different states of the rocket
 *
 *
 **************************************************************/

#ifndef DECOMPRESSION_INCLUDED
#define DECOMPRESSION_INCLUDED

#include <inttypes.h>

unsigned int *unpack_poweron(uint64_t poweron_data);

unsigned int *unpack_launchready(uint64_t *launchready_data);

unsigned int *unpack_launchmode(uint64_t *launchmode_data);

unsigned int *unpack_recovery(uint64_t *recovery_data);

#endif