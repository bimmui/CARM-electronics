/**************************************************************
 *
 *                     BBsetup.h
 *
 *     Author(s):  Daniel Opara,
 *     Date:       1/6/2024
 *
 *     Overview: Contains functions for setting up configurations for the breakout
 *                  boards connected to the Feather M0, this includes the sensor breakout
 *                  boards, the SD card module, GPS module, etc.
 *
 *
 **************************************************************/

#ifndef BB_SETUP_H
#define BB_SETUP_H

#include <Adafruit_LSM9DS1.h>
#include <Adafruit_BMP3XX.h>
#include <Adafruit_MCP9808.h>
#include <Adafruit_GPS.h>
#include <SD.h>

bool setup_IMU(Adafruit_LSM9DS1 &lsm_obj);
bool setup_BMP(Adafruit_BMP3XX &bmp_obj);
bool setup_tempsens(Adafruit_MCP9808 &tempsensor_obj, uint8_t address);
bool setup_SD();
bool setup_GPS(Adafruit_GPS &gps_obj);

#endif