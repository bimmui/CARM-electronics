#ifndef UNTRANSFORMED_H
#define UNTRANSFORMED_H

#include <inttypes.h>

struct powerondata
{
    int gps_antenna_status;
    int gps_num_satellites;
    int gps_fix;
    int gps_quality;
    float temperature_avbay;
    float temperature_engbay;
    float external_temp;
    int curr_state;
    uint16_t failures;
};

struct launchreadydata
{
    int gps_antenna_status;
    int gps_num_satellites;
    int gps_fix;
    int gps_quality;
    float gps_lat;
    float gps_long;
    float gps_speed;
    float gps_altitude;
    float temperature_avbay;
    float temperature_engbay;
    float external_temp;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float accel_x;
    float accel_y;
    float accel_z;
    float mag_x;
    float mag_y;
    float mag_z;
    float vert_velo;
    float altitude;
    int curr_state;
    uint16_t failures;
};

struct launchmodedata
{
    unsigned timestamp;
    int gps_antenna_status;
    int gps_num_satellites;
    int gps_fix;
    int gps_quality;
    float gps_lat;
    float gps_long;
    float gps_speed;
    float gps_altitude;
    float temperature_avbay;
    float temperature_engbay;
    float external_temp;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float accel_x;
    float accel_y;
    float accel_z;
    float mag_x;
    float mag_y;
    float mag_z;
    float vert_velo;
    float altitude;
    int curr_state;
    uint16_t failures;
};

struct recoverydata
{
    int gps_antenna_status;
    int gps_num_satellites;
    int gps_fix;
    int gps_quality;
    float temperature_avbay;
    float temperature_engbay;
    float external_temp;
    float gps_lat;
    float gps_long;
    int curr_state;
    uint16_t failures;
};

#endif