/**************************************************************
 *
 *                     carm_computer_launch1.ino
 *
 *     Author(s):  Daniel Opara, Kenny Chua
 *     Date:       1/6/2024
 *
 *     Overview: Driver code for the rocket computer. Gets readings from the sensors
 *                  connected to it and writes it to an SD card.
 *
 *
 **************************************************************/

#include <Wire.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS1.h> // IMU module
#include "Adafruit_BMP3XX.h"  // BMP module
#include "Adafruit_MCP9808.h" // Temp sensor module
#include <Adafruit_GPS.h>     // GPS module
#include "utils.h"
#include "def.h"
#include "BBManager.h"
#include "DLTransforms.h"
#include "compression.h"
#include "decompression.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Defining sensor objects and structs
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
BBManager bboard_manager = BBManager();

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Defining Rocket States and relevant indicators
// - - - - - - - - - - - - - - - - - - - - - - - - - - -

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Function contracts
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//! TODO: Test this
void switchSPIDevice(int cs_pin);

// TODO: Move this to bbman cpp file
// void readGPS(SensorData &sensor_data)
// {
//     // gps reading
//     GPS.read();
//     if (GPS.newNMEAreceived())
//     {
//         if (!GPS.parse(GPS.lastNMEA()))
//         { // this also sets the newNMEAreceived() flag to false
//             return;
//         }
//     }
//     if (GPS.fix)
//     {
//         sensor_data.gps_fix = GPS.fix;
//         sensor_data.gps_quality = GPS.fixquality;
//         sensor_data.gps_num_satellites = (int)GPS.satellites;
//         sensor_data.gps_antenna_status = (int)GPS.antenna;
//         sensor_data.gps_lat = convertToDecimalDegrees(GPS.latitude, GPS.lat);
//         sensor_data.gps_long = convertToDecimalDegrees(GPS.longitude, GPS.lon);
//         sensor_data.gps_speed = GPS.speed;
//         sensor_data.gps_angle = GPS.angle;
//         sensor_data.gps_altitude = GPS.altitude;
//     }
//     else
//     {
//         sensor_data.gps_fix = GPS.fix;
//         sensor_data.gps_quality = 1;
//         sensor_data.gps_num_satellites = 1;
//         sensor_data.gps_antenna_status = 1;
//         sensor_data.gps_lat = 1;
//         sensor_data.gps_long = 1;
//         sensor_data.gps_speed = 1;
//         sensor_data.gps_angle = 1;
//         sensor_data.gps_altitude = 1;
//     }
// }

void setup()
{

    // pinMode(J1, OUTPUT);
    // pinMode(J2, OUTPUT);

    // Switch the spi device before setting up the SD card module
    switchSPIDevice(SD_CS);
}

void loop()
{
    bboard_manager.readSensorData();
    state_determiner(bboard_manager);
    bboard_manager.writeSensorData();
    switch (bboard_manager.curr_state)
    {
    case state::POWER_ON:
    {
        unsigned int *poweron_d = transform_poweron(bboard_manager);
        uint64_t poweron_word = pack_poweron(poweron_d);
        break;
    }
    case state::LAUNCH_READY:
    {
        unsigned int *launchready_d = transform_launchready(bboard_manager);
        uint64_t *launchready_words = pack_launchready(launchready_d);
        break;
    }
    default:
        // code block
        break;
    }
}

/*
 * switchSPIDevice
 * Parameters: An integer that represents the CS pin of an SPI device
 * Purpose: Switches the SPI device to communicate with.
 * Returns: Nothing
 * Notes: The Feather can only "talk" to one device at a time so we much switch
 *          the CS pin that is being listened to before using the respective device
 */
void switchSPIDevice(int cs_pin)
{
    // we want to use radio in this
    if (cs_pin == RFM95_CS)
    {
        digitalWrite(SD_CS, HIGH);
        digitalWrite(RFM95_CS, LOW);
    }
    // we want to use the sd card module in this
    if (cs_pin == SD_CS)
    {
        digitalWrite(RFM95_CS, HIGH);
        digitalWrite(SD_CS, LOW);
    }
}
