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
#include "StateDetermination.h"
#include "DLTransforms.h"
#include "untransformed.h"
#include "decompression.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Initializing revelant objects and structs
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Singleton instance of the radio driver
RH_RF95 rf95(RFM96_CS, RFM96_INT);

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Function contracts
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
uint16_to_binary_string(uint16_t num, char *result);

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
    uint64_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    Serial.println("Waiting for reply...");
    if (rf95.waitAvailableTimeout(1000))
    {
        // Should be a reply message for us now
        if (rf95.recv(buf, &len))
        {
            // the first word will always have the state in the first four bits
            unsigned int *launchmode_d = unpack_launchmode(buf[0]);
            launchmodedata launchmode_stuct = unpack_launchmode(launchmode_d); // TODO: rename the var

            // Print all the data to serial so the parser can read it and send it to the db and dashboard
            Serial.println(launchready_stuct.curr_state);
            Serial.println(launchready_stuct.gps_num_satellites);
            Serial.println(launchready_stuct.gps_long);
            Serial.println(launchready_stuct.gps_lat);
            Serial.println(launchready_stuct.gyro_x);
            Serial.println(launchready_stuct.accel_y);
            Serial.println(launchready_stuct.gyro_y);
            Serial.println(launchready_stuct.vert_velo);
            Serial.println(launchready_stuct.gyro_z);

            Serial.println(launchready_stuct.accel_x);
            Serial.println(launchready_stuct.altitude);
            Serial.println(launchready_stuct.gps_fix);
            Serial.println(launchready_stuct.external_temp);
            Serial.println(launchready_stuct.temperature_avbay);
            Serial.println(launchready_stuct.accel_z);
            Serial.println(launchready_stuct.mag_y);
            Serial.println(launchready_stuct.mag_z);

            char failures_bits_str[17];
            uint16_to_binary_string(launchready_stuct.failures, &failures_bits_str);
            Serial.println(failures_bits_str);

            Serial.println(launchready_stuct.gps_speed);
            Serial.println(launchready_stuct.gps_altitude);
            Serial.println(launchready_stuct.gps_quality);
            Serial.println(launchready_stuct.temperature_engbay);
            Serial.println(launchready_stuct.gps_antenna_status);
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