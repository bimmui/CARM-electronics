/**************************************************************
 *
 *                     def.h
 *
 *     Author(s):  Daniel Opara
 *     Date:       2/15/2024
 *
 *     Overview: Defines constants for pins, function params, etc.
 *
 *
 **************************************************************/

// #define RFM95_CS 8
// #define RFM95_RST 4
// #define RFM95_INT 7

#pragma once

#if defined(ADAFRUIT_FEATHER_M0) || defined(ADAFRUIT_FEATHER_M0_EXPRESS) || defined(ARDUINO_SAMD_FEATHER_M0) // Feather M0 w/Radio
#define RFM95_CS 8
#define RFM95_INT 3
#define RFM95_RST 4

#endif

//! TODO: Change the frequencies
#define RF95_FREQ_1 422.0
// #define RF95_FREQ_2 466.0
// #define RF96_FREQ_3 423.0
// #define RF96_FREQ_4 423.0
// #define RF96_FREQ_5 423.0
// #define RF96_FREQ_6 423.0
// #define RF96_FREQ_7 423.0
// #define RF96_FREQ_9 423.0
// #define RF96_FREQ_9 423.0
// #define RF96_FREQ_10 423.0

// BMP_SCK == SCL
#define BMP_SCK 18
// BMP_CS == CS
#define BMP_CS 17
#define SEALEVELPRESSURE_HPA (1012.3)
#define SD_CS 13
#define GPSSerial Serial1
#define GPSECHO false
#define BUZZER_PIN 9