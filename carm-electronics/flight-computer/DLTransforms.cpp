/**************************************************************
 *
 *                     DLTransforms.cpp
 *
 *     Author(s):  Daniel Opara
 *     Date:       3/20/2024
 *
 *     Overview: Implementation of DLTransforms.h
 *
 *
 **************************************************************/

#include "DLTransforms.h"

const float EXT_TEMP_SPACING = 0.0683927699072;
const float INT_TEMP_SPACING = 0.0620420127015;
const float ALTITUDE_SPACING = 0.0999481185339;
const float VERT_VELO_SPACING = 0.0122074037904;
const float ACCEL_Z_SPACING = 0.0635075720567;
const float ACCEL_XY_SPACING = 0.0489236790607;
const float MAG_FORCE_SPACING = 0.0195694716243;
const float GYRO_XY_SPACING = 0.0027465846506;
const float GYRO_Z_SPACING = 0.00549320597234;
const float GPS_SPEED_SPACING = 0.0999481185339;

unsigned int serialize_dlt(unsigned int bit_count, int n_min, int n_max, float reading, float m_spacing)
{
  unsigned int serialized_data = floor((reading - n_min) / m_spacing);
  return serialized_data;
}

float deserialize_dlt(unsigned int serialized, int n_min, float m_spacing)
{
  float deserialized_data = ((serialized * m_spacing) + n_min);
  return deserialized_data;
}

/*!
 * BIG TODO: CHECK IF THE EXPLICIT CASTING IS NEEDED FOR THE FUNCS BELOW!!
 *
 */

/*
 * transform_poweron
 * Parameters: An array of unsigned ints that have gone through DLT
 * Returns: A array of unsigned ints
 * Purpose: Packs the transformed data into 64 bit word(s)
 * Notes: Follow guidelines of the POWER ON bitfield schema
 *
 */
unsigned int *transform_poweron(BBManager bbman)
{
  // there are 9 fields in this schema
  unsigned int transformed_values[8];

  transformed_values[0] = static_cast<unsigned int>(bbman.curr_state);
  transformed_values[1] = serialize_dlt(11, -15, 125, bbman.external_temp, EXT_TEMP_SPACING);
  transformed_values[2] = serialize_dlt(11, 0, 127, bbman.temperature_engbay, INT_TEMP_SPACING);
  transformed_values[3] = serialize_dlt(11, 0, 127, bbman.temperature_avbay, INT_TEMP_SPACING);
  transformed_values[4] = static_cast<unsigned int>(bbman.gps_quality);
  transformed_values[5] = static_cast<unsigned int>(bbman.gps_fix);
  transformed_values[6] = static_cast<unsigned int>(bbman.gps_num_satellites);
  transformed_values[7] = static_cast<unsigned int>(bbman.gps_antenna_status);
  transformed_values[8] = static_cast<unsigned int>(bbman.errors_bit_mask);

  return transformed_values;
}

/*
 * transform_launchready
 * Parameters: An array of unsigned ints that have gone through DLT
 * Returns: A array of unsigned ints
 * Purpose: Packs the transformed data into 64 bit word(s)
 * Notes: Follow guidelines of the LAUNCH READY bitfield schema
 *
 */
unsigned int *transform_launchready(BBManager bbman)
{
  // there are 26 fields in this schema
  unsigned int transformed_values[26];

  // STATE
  transformed_values[0] = static_cast<unsigned int>(bbman.curr_state);

  // GPS LINK QUALITY; NUM OF SATELITTES LIKED TO MODULE
  transformed_values[1] = bbman.gps_num_satellites;

  // GPS LONGITUDE COORDINATE SIGN (pos or negative)
  transformed_values[2] = (bbman.gps_long > 0) ? 1 : ((bbman.gps_long < 0) ? 0 : 0); // if > 0, then set to 1, otherwise set to 0

  // GPS LONGITUDE COORDINATES
  if (bbman.gps_long > 0)
  {
    transformed_values[3] = static_cast<unsigned int>(bbman.gps_long * 1000000);
  }
  else if (bbman.gps_long < 0)
  {
    transformed_values[3] = static_cast<unsigned int>(bbman.gps_long * 1000000 * -1);
  }
  else
  {
    transformed_values[3] = 0;
  }

  // GPS LATITUDE COORDINATE SIGN (pos or negative)
  transformed_values[4] = (bbman.gps_lat > 0) ? 1 : ((bbman.gps_lat < 0) ? 0 : 0);

  // GPS LATITUDE COORDINATES
  if (bbman.gps_lat > 0)
  {
    transformed_values[5] = static_cast<unsigned int>(bbman.gps_lat * 1000000);
  }
  else if (bbman.gps_lat < 0)
  {
    transformed_values[5] = static_cast<unsigned int>(bbman.gps_lat * 1000000 * -1);
  }
  else
  {
    transformed_values[5] = 0;
  }

  // GYROSCOPE X-AXIS MEASUREMENTS
  transformed_values[6] = serialize_dlt(20, -1440, 1440, bbman.gyro_x, GYRO_XY_SPACING);

  // ACCELERATION Y-AXIS MEASUREMENTS
  transformed_values[7] = serialize_dlt(9, 0, 25, bbman.accel_y, ACCEL_XY_SPACING);

  // GYROSCOPE Y-AXIS MEASUREMENTS
  transformed_values[8] = serialize_dlt(20, -1440, 1440, bbman.gyro_y, GYRO_XY_SPACING);

  // KF VERTICAL VELOCITY CALCULATIONS
  transformed_values[9] = serialize_dlt(15, -50, 350, bbman.vert_velocity, VERT_VELO_SPACING);

  // GYROSCOPE Z-AXIS MEASUREMENTS
  transformed_values[10] = serialize_dlt(17, -360, 360, bbman.gyro_z, GYRO_Z_SPACING);

  // ACCELERATION X-AXIS MEASUREMENTS
  transformed_values[11] = serialize_dlt(9, 0, 25, bbman.accel_x, ACCEL_XY_SPACING);

  // ALTITUDE MEASUREMENTS
  transformed_values[12] = serialize_dlt(15, 0, 3275, bbman.accel_x, ALTITUDE_SPACING);

  // GPS FIX
  transformed_values[13] = static_cast<unsigned int>(bbman.gps_fix);

  // EXTERNAL TEMPERATURE
  transformed_values[14] = serialize_dlt(11, -15, 125, bbman.external_temp, EXT_TEMP_SPACING);

  // INTERNAL TEMPERATURE: AVIONICS BAY
  transformed_values[15] = serialize_dlt(11, 0, 127, bbman.temperature_avbay, INT_TEMP_SPACING);

  // ACCELERATION Z-AXIS MEASUREMENTS
  transformed_values[16] = serialize_dlt(11, -30, 100, bbman.accel_z, ACCEL_Z_SPACING);

  // MAGNETIC FORCE X-AXIS MEASUREMENTS
  transformed_values[17] = serialize_dlt(11, -5, 5, bbman.mag_x, MAG_FORCE_SPACING);

  // MAGNETIC FORCE Y-AXIS MEASUREMENTS
  transformed_values[18] = serialize_dlt(11, -5, 5, bbman.mag_y, MAG_FORCE_SPACING);

  // MAGNETIC FORCE Z-AXIS MEASUREMENTS
  transformed_values[19] = serialize_dlt(11, -5, 5, bbman.mag_z, MAG_FORCE_SPACING);

  // ERROR FLAGS
  transformed_values[20] = static_cast<unsigned int>(bbman.errors_bit_mask);

  // GPS SPEED MEASUREMENTS
  transformed_values[21] = serialize_dlt(10, 0, 70, bbman.gps_speed, GPS_SPEED_SPACING);

  // GPS ALTITUDE MEASUREMENTS
  transformed_values[22] = serialize_dlt(15, 0, 3275, bbman.gps_altitude, ALTITUDE_SPACING);

  // GPS SIGNAL QUALITY
  transformed_values[23] = static_cast<unsigned int>(bbman.gps_quality);

  // INTERNAL TEMPERATURE: ENGINE BAY
  transformed_values[24] = serialize_dlt(11, 0, 127, bbman.temperature_engbay, INT_TEMP_SPACING);

  // GPS ANTENNA STATUS
  transformed_values[25] = static_cast<unsigned int>(bbman.gps_antenna_status);

  return transformed_values;
}

/*
 * transform_launchmode
 * Parameters: An array of unsigned ints that have gone through DLT
 * Returns: A array of unsigned ints
 * Purpose: Packs the transformed data into 64 bit word(s)
 * Notes: Follow guidelines of the LAUNCH MODE bitfield schema
 *
 */
unsigned int *transform_launchmode(BBManager bbman)
{
  // there are 27 fields in this schema
  unsigned int transformed_values[27];

  // STATE
  transformed_values[0] = static_cast<unsigned int>(bbman.curr_state);

  // GPS LINK QUALITY; NUM OF SATELITTES LIKED TO MODULE
  transformed_values[1] = bbman.gps_num_satellites;

  // GPS LONGITUDE COORDINATE SIGN (pos or negative)
  transformed_values[2] = (bbman.gps_long > 0) ? 1 : ((bbman.gps_long < 0) ? 0 : 0); // if > 0, then set to 1, otherwise set to 0

  // GPS LONGITUDE COORDINATES
  if (bbman.gps_long > 0)
  {
    transformed_values[3] = static_cast<unsigned int>(bbman.gps_long * 1000000);
  }
  else if (bbman.gps_long < 0)
  {
    transformed_values[3] = static_cast<unsigned int>(bbman.gps_long * 1000000 * -1);
  }
  else
  {
    transformed_values[3] = 0;
  }

  // GPS LATITUDE COORDINATE SIGN (pos or negative)
  transformed_values[4] = (bbman.gps_lat > 0) ? 1 : ((bbman.gps_lat < 0) ? 0 : 0);

  // GPS LATITUDE COORDINATES
  if (bbman.gps_lat > 0)
  {
    transformed_values[5] = static_cast<unsigned int>(bbman.gps_lat * 1000000);
  }
  else if (bbman.gps_lat < 0)
  {
    transformed_values[5] = static_cast<unsigned int>(bbman.gps_lat * 1000000 * -1);
  }
  else
  {
    transformed_values[5] = 0;
  }

  // GYROSCOPE X-AXIS MEASUREMENTS
  transformed_values[6] = serialize_dlt(20, -1440, 1440, bbman.gyro_x, GYRO_XY_SPACING);

  // GPS ALTITUDE MEASUREMENTS
  transformed_values[7] = serialize_dlt(15, 0, 3275, bbman.gps_altitude, ALTITUDE_SPACING);

  // ACCELERATION X-AXIS MEASUREMENTS
  transformed_values[8] = serialize_dlt(9, 0, 25, bbman.accel_x, ACCEL_XY_SPACING);

  // GYROSCOPE Y-AXIS MEASUREMENTS
  transformed_values[9] = serialize_dlt(20, -1440, 1440, bbman.gyro_y, GYRO_XY_SPACING);

  // TIMESTAMP
  transformed_values[10] = bbman.curr_launch_time;

  // GYROSCOPE Z-AXIS MEASUREMENTS
  transformed_values[11] = serialize_dlt(17, -360, 360, bbman.gyro_z, GYRO_Z_SPACING);

  // ALTITUDE MEASUREMENTS
  transformed_values[12] = serialize_dlt(15, 0, 3275, bbman.accel_x, ALTITUDE_SPACING);

  // GPS ANTENNA STATUS
  transformed_values[13] = static_cast<unsigned int>(bbman.gps_antenna_status);

  // EXTERNAL TEMPERATURE
  transformed_values[14] = serialize_dlt(11, -15, 125, bbman.external_temp, EXT_TEMP_SPACING);

  // ACCELERATION Y-AXIS MEASUREMENTS
  transformed_values[15] = serialize_dlt(9, 0, 25, bbman.accel_y, ACCEL_XY_SPACING);

  // INTERNAL TEMPERATURE: AVIONICS BAY
  transformed_values[16] = serialize_dlt(11, 0, 127, bbman.temperature_avbay, INT_TEMP_SPACING);

  // INTERNAL TEMPERATURE: ENGINE BAY
  transformed_values[17] = serialize_dlt(11, 0, 127, bbman.temperature_engbay, INT_TEMP_SPACING);

  // ACCELERATION Z-AXIS MEASUREMENTS
  transformed_values[18] = serialize_dlt(11, -30, 100, bbman.accel_z, ACCEL_Z_SPACING);

  // MAGNETIC FORCE X-AXIS MEASUREMENTS
  transformed_values[19] = serialize_dlt(11, -5, 5, bbman.mag_x, MAG_FORCE_SPACING);

  // MAGNETIC FORCE Y-AXIS MEASUREMENTS
  transformed_values[20] = serialize_dlt(11, -5, 5, bbman.mag_y, MAG_FORCE_SPACING);

  // MAGNETIC FORCE Z-AXIS MEASUREMENTS
  transformed_values[21] = serialize_dlt(11, -5, 5, bbman.mag_z, MAG_FORCE_SPACING);

  // ERROR FLAGS
  transformed_values[22] = static_cast<unsigned int>(bbman.errors_bit_mask);

  // GPS SPEED MEASUREMENTS
  transformed_values[23] = serialize_dlt(10, 0, 70, bbman.gps_speed, GPS_SPEED_SPACING);

  // KF VERTICAL VELOCITY CALCULATIONS
  transformed_values[24] = serialize_dlt(15, -50, 350, bbman.vert_velocity, VERT_VELO_SPACING);

  // GPS SIGNAL QUALITY
  transformed_values[25] = static_cast<unsigned int>(bbman.gps_quality);

  // GPS FIX
  transformed_values[26] = static_cast<unsigned int>(bbman.gps_fix);

  return transformed_values;
}

/*
 * transform_recovery
 * Parameters: An array of unsigned ints that have gone through DLT
 * Returns: A array of unsigned ints
 * Purpose: Packs the transformed data into 64 bit word(s)
 * Notes: Follow guidelines of the RECOVERY MODE bitfield schema
 *
 */
unsigned int *transform_recovery(BBManager bbman)
{
  // there are 13 fields in this schema
  unsigned int transformed_values[13];

  transformed_values[0] = static_cast<unsigned int>(bbman.curr_state);
  transformed_values[1] = bbman.gps_num_satellites;

  // GPS LONGITUDE COORDINATE SIGN (pos or negative)
  transformed_values[2] = (bbman.gps_long > 0) ? 1 : ((bbman.gps_long < 0) ? 0 : 0); // if > 0, then set to 1, otherwise set to 0

  // GPS LONGITUDE COORDINATES
  if (bbman.gps_long > 0)
  {
    transformed_values[3] = static_cast<unsigned int>(bbman.gps_long * 1000000);
  }
  else if (bbman.gps_long < 0)
  {
    transformed_values[3] = static_cast<unsigned int>(bbman.gps_long * 1000000 * -1);
  }
  else
  {
    transformed_values[3] = 0;
  }

  // GPS LATITUDE COORDINATE SIGN (pos or negative)
  transformed_values[4] = (bbman.gps_lat > 0) ? 1 : ((bbman.gps_lat < 0) ? 0 : 0);

  // GPS LATITUDE COORDINATES
  if (bbman.gps_lat > 0)
  {
    transformed_values[5] = static_cast<unsigned int>(bbman.gps_lat * 1000000);
  }
  else if (bbman.gps_lat < 0)
  {
    transformed_values[5] = static_cast<unsigned int>(bbman.gps_lat * 1000000 * -1);
  }
  else
  {
    transformed_values[5] = 0;
  }

  transformed_values[6] = serialize_dlt(11, -15, 125, bbman.external_temp, EXT_TEMP_SPACING);
  transformed_values[7] = serialize_dlt(11, 0, 127, bbman.temperature_engbay, INT_TEMP_SPACING);
  transformed_values[8] = serialize_dlt(11, 0, 127, bbman.temperature_avbay, INT_TEMP_SPACING);
  transformed_values[9] = static_cast<unsigned int>(bbman.gps_fix);
  transformed_values[10] = static_cast<unsigned int>(bbman.errors_bit_mask);
  transformed_values[11] = static_cast<unsigned int>(bbman.gps_quality);
  transformed_values[12] = static_cast<unsigned int>(bbman.gps_antenna_status);

  return transformed_values;
}