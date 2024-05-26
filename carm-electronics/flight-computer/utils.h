/**************************************************************
 *
 *                     utils.h
 *
 *     Author(s):  Daniel Opara,
 *     Date:       1/9/2024
 *
 *     Overview: A collection of utility funtions
 *
 *
 **************************************************************/

#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <inttypes.h>
#include "def.h"
#include "bitpack.h"
#include "StateDetermination.h"

const int MAX_PACKET_LEN = 5;

float convert_to_decimaldegrees(float coordinate, char direction);
float calculate_average_acceleration(float x_accel, float y_accel, float z_accel);
float convert_knots_to_mps(float knots);
uint16_t flip_bit(unsigned n, unsigned p, unsigned b);
bool check_bit(int n, int p);
void transmit(RH_RF95 rf96, uint64_t packet[], uint8_t packet_len);
uint64_t *receive(RH_RF95 rf96, uint8_t packet_len);
state get_state_from_bits(uint64_t packet);
unsigned short fcs_calc(unsigned char *data, int len);

#endif