/**************************************************************
 *
 *                     BBManager.h
 *
 *     Author(s):  Daniel Opara
 *     Date:       3/20/2024
 *
 *     Overview: A class for setting up configurations for the breakout
 *                  boards connected to the Feather M0 and reading values from
 *                  the connected sensors
 *
 *
 **************************************************************/

#ifndef BB_MANAGER_H
#define BB_MANAGER_H

#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS1.h> // IMU module
#include "Adafruit_BMP3XX.h"  // BMP module
#include "Adafruit_MCP9808.h" // Temp sensor module
#include <Adafruit_GPS.h>     // GPS module
#include "def.h"

#include "StateDetermination.h"

class BBManager
{
public:
    BBManager();
    ~BBManager();
    void setSensors(Adafruit_LSM9DS1 &lsm_obj, Adafruit_BMP3XX &bmp_obj,
                    Adafruit_MCP9808 &tempsensor_obj1, Adafruit_MCP9808 &tempsensor_obj2,
                    Adafruit_GPS &gps_obj);
    void readSensorData();
    void writeSensorData(File &data_stream, File &error_stream);
    void initDatalog(File &file_stream);
    unsigned launch_start_time;
    unsigned curr_launch_time;
    float temperature_avbay;
    float temperature_engbay;
    float external_temp;
    float barometer_temp;
    float pressure;
    float altitude;

    // kalman filtered values
    float k_vert_velocity;
    float k_vert_acceleration;
    float k_altitude;

    float accel_x;
    float accel_y;
    float accel_z;
    float mag_x;
    float mag_y;
    float mag_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float gps_lat;
    float gps_long;
    float gps_speed;
    float gps_angle;
    float gps_altitude;
    int gps_fix;
    int gps_quality;
    int gps_num_satellites;
    int gps_antenna_status;
    state curr_state;
    uint16_t failure_flags;
    // TODO: make functions that edit the bit mask whenever an error occurs

private:
    Adafruit_LSM9DS1 *lsm;               // imu
    Adafruit_BMP3XX *bmp;                // barometric pressure sensor
    Adafruit_MCP9808 *tempsensor_avbay;  // avionics bay thermocouple
    Adafruit_MCP9808 *tempsensor_engbay; // engine bay thermocouple
    Adafruit_GPS *gps;                   // gps module
};

#endif