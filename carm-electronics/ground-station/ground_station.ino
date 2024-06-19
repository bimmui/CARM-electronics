/**************************************************************
 *
 *                     carm_computer_launch1.ino
 *
 *     Author(s):  Daniel Opara
 *     Date:       1/6/2024
 *
 *     Overview: Driver code for the rocket computer. Gets readings from the sensors
 *                  connected to it and writes it to an SD card.
 *
 *
 **************************************************************/

#include <RH_RF95.h>
#include "DLTransforms.h"
#include "untransformed.h"
#include "decompression.h"

#if defined(ADAFRUIT_FEATHER_M0) || defined(ADAFRUIT_FEATHER_M0_EXPRESS) || defined(ARDUINO_SAMD_FEATHER_M0) // Feather M0 w/Radio
#define RFM95_CS 8
#define RFM95_INT 3
#define RFM95_RST 4

#endif

#define RF95_FREQ 433.0
static const unsigned DECIMAL_COUNT = 4;
static const unsigned GPS_DECIMAL_COUNT = 6;

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Initializing revelant objects and structs
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Function contracts
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
void uint16_to_binary_string(uint16_t num, char *result);

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, HIGH);

    Serial.begin(115200);
    while (!Serial)
        delay(1);
    delay(100);

    Serial.println("Feather LoRa RX Test!");

    // manual reset
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);

    while (!rf95.init())
    {
        Serial.println("LoRa radio init failed");
        Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
        while (1)
            ;
    }
    Serial.println("LoRa radio init OK!");

    // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
    if (!rf95.setFrequency(RF95_FREQ))
    {
        Serial.println("setFrequency failed");
        while (1)
            ;
    }
    Serial.print("Set Freq to: ");
    Serial.println(RF95_FREQ);

    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

    // The default transmitter power is 13dBm, using PA_BOOST.
    // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
    // you can set transmitter powers from 5 to 23 dBm:
    rf95.setTxPower(23, false);
}

void loop()
{
    uint64_t buf[5];
    uint8_t len = sizeof(buf);

    if (rf95.available())
    {
        // Should be a reply message for us now
        if (rf95.recv((uint8_t *)buf, &len))
        {
            // the first word will always have the state in the first four bits
            unsigned int *launchmode_d = unpack_noschema(buf);
            launchmodedata launchdata = untransform_launchmode(launchmode_d); // TODO: rename the var

            // Print all the data to serial so the parser can read it and send it to the db and dashboard
            Serial.print(launchdata.curr_state);
            Serial.print(",");
            Serial.print(launchdata.gps_num_satellites);
            Serial.print(",");
            Serial.print(float(launchdata.gps_long / 1000000), GPS_DECIMAL_COUNT);
            Serial.print(",");
            Serial.print(float(launchdata.gps_lat / 1000000), GPS_DECIMAL_COUNT);
            Serial.print(",");
            Serial.print(launchdata.gyro_x, DECIMAL_COUNT);
            Serial.print(",");
            Serial.print(launchdata.accel_y, DECIMAL_COUNT);
            Serial.print(",");
            Serial.print(launchdata.gyro_y, DECIMAL_COUNT);
            Serial.print(",");
            Serial.print(launchdata.vert_velo, DECIMAL_COUNT);
            Serial.print(",");
            Serial.print(launchdata.gyro_z, DECIMAL_COUNT);
            Serial.print(",");

            Serial.print(launchdata.accel_x, DECIMAL_COUNT);
            Serial.print(",");
            Serial.print(launchdata.altitude, DECIMAL_COUNT);
            Serial.print(",");
            Serial.print(launchdata.gps_fix);
            Serial.print(",");
            // Serial.print(launchdata.external_temp, DECIMAL_COUNT);
            // Serial.print(",");
            Serial.print(launchdata.temperature_avbay, DECIMAL_COUNT);
            Serial.print(",");
            Serial.print(launchdata.accel_z, DECIMAL_COUNT);
            Serial.print(",");
            Serial.print(launchdata.mag_y, DECIMAL_COUNT);
            Serial.print(",");
            Serial.print(launchdata.mag_z, DECIMAL_COUNT);
            Serial.print(",");

            char failures_bits_str[17];
            uint16_to_binary_string(launchdata.failures, failures_bits_str);
            Serial.print(failures_bits_str);
            Serial.print(",");

            Serial.print(launchdata.gps_speed, DECIMAL_COUNT);
            Serial.print(",");
            Serial.print(launchdata.gps_altitude, DECIMAL_COUNT);
            Serial.print(",");
            Serial.print(launchdata.gps_quality);
            // Serial.print(",");
            // Serial.print(launchdata.temperature_engbay, DECIMAL_COUNT);
            Serial.print(",");
            Serial.println(launchdata.gps_antenna_status);
        }
    }
}

void uint16_to_binary_string(uint16_t num, char *result)
{
    for (int i = 15; i >= 0; i--)
    {
        result[15 - i] = ((num & (1 << i)) ? '1' : '0');
    }
    result[16] = '\0'; // Null-terminate the string
}