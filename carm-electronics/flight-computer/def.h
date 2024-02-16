/**************************************************************
 *
 *                     def.h
 *
 *     Author(s):  Daniel Opara
 *     Date:       2/15/2024
 *
 *     Overview: Defines constants for pins, functions, etc.
 *
 *
 **************************************************************/

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define RF95_FREQ 433.0

// Ejection Charge Pin #1 (J1)
#define J1 24
// Ejection Charge Pin #2 (J2)
#define J2 23
// BMP_SCK == SCL
#define BMP_SCK 18
// BMP_CS == CS
#define BMP_CS 17
#define SEALEVELPRESSURE_HPA (1013.25)
#define SD_CS 13
#define GPSSerial Serial1
// queue constants
#define IMPLEMENTATION FIFO
#define QUEUE_MAX_LENGTH 10