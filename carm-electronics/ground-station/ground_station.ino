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

RFM69 radio;

void setup()
{
    // Initialize radio and database
    radio.init();
    db.open();
    db.createTable("sensor_data", "time REAL, temperature_avbay REAL, ...");
    Serial.begin(9600); // Initialize serial communication
}

void loop()
{
    SensorData data;
    if (radio.receive(data, sizeof(SensorData)))
    {
        // Store data in SQL database
        db.insert("sensor_data", data);

        // Pass data to serial port
        Serial.write((uint8_t *)&data, sizeof(SensorData));
    }
}

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

#include <Wire.h>
#include <SD.h>
#include "def.h"
#include "utils.h"
#include "BBManager.h"
#include "DLTransforms.h"
#include "compression.h"
#include "decompression.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Initializing revelant objects and structs
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
BBManager bboard_manager = BBManager();
// Singleton instance of the radio driver
RH_RF95 rf96(RFM96_CS, RFM96_INT);

void setup()
{
    switchSPI_device(SD_CS);
}

void loop()
{
    bboard_manager.readSensorData();
    state_determiner(bboard_manager);
    switchSPI_device(SD_CS);
    bboard_manager.writeSensorData();
    switchSPI_device(RFM96_CS);
    switch (bboard_manager.curr_state)
    {
    case state::POWER_ON:
    {
        unsigned int *poweron_d = transform_poweron(bboard_manager);
        uint64_t[1] poweron_word = {pack_poweron(poweron_d)};
        transmit(rf96, poweron_word, 1);
        receive(rf96, 1) break;
    }
    case state::LAUNCH_READY:
    {
        unsigned int *launchready_d = transform_launchready(bboard_manager);
        uint64_t *launchready_words = pack_launchready(launchready_d);
        transmit(rf96, launchready_words, 5);
        receive(rf96, 1);
        break;
    }
    default:
        // code block
        break;
    }
}