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
#include "untransformed.h"

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

/*
 * serialize_dlt
 * Parameters: Number of bits to allocate for a data field, minimum value range for the data field,
 *              maximum data value for the data field, the actual sensor reading, and the DLT spacing for
 *              the data field, respectively
 * Returns: The transformed value of the sensor reading; what the sensor reading is in DLT space
 * Purpose: Transforms sensor readings into unsigned integers that exist in DLT space
 * Notes: Please read the transmission protocol for more details on how this works
 */
unsigned int serialize_dlt(unsigned int bit_count, int n_min, int n_max, float reading, float m_spacing)
{
  unsigned int serialized_data = floor((reading - n_min) / m_spacing);
  return serialized_data;
}

/*
 * deserialize_dlt
 * Parameters: A value that exists in DLT space, minimum value range for the data field, and
 *              the DLT spacing for the data field, respectively
 * Returns: The true (lossy) sensor readings
 * Purpose: Restores float sensor readings from DLT transformed values
 * Notes: Please read the transmission protocol for more details on how this works
 */
float deserialize_dlt(unsigned int serialized, int n_min, float m_spacing)
{
  float deserialized_data = ((serialized * m_spacing) + n_min);
  return deserialized_data;
}

/*!
 * BIG TODO: CHECK IF THE EXPLICIT CASTING IS NEEDED FOR THE FUNCS BELOW!!
 */

/*
 * transform_poweron
 * Parameters: A BBManager object
 * Returns: A array of unsigned ints
 * Notes:
 *      - Follow guidelines of the POWER ON bitfield schema,
 *      - Some fields don't need to undergo DLT and simply need to
 *            be recasted
 *      - The BBManager object is used to get current sensor readings
 */
unsigned int *transform_poweron(BBManager bbman)
{
  // there are 9 fields in this schema
  static unsigned int transformed_values[9];

  transformed_values[0] = static_cast<unsigned int>(bbman.curr_state);
  transformed_values[1] = serialize_dlt(11, -15, 125, bbman.external_temp, EXT_TEMP_SPACING);
  transformed_values[2] = serialize_dlt(11, 0, 127, bbman.temperature_engbay, INT_TEMP_SPACING);
  transformed_values[3] = serialize_dlt(11, 0, 127, bbman.temperature_avbay, INT_TEMP_SPACING);
  transformed_values[4] = static_cast<unsigned int>(bbman.gps_quality);
  transformed_values[5] = static_cast<unsigned int>(bbman.gps_fix);
  transformed_values[6] = static_cast<unsigned int>(bbman.gps_num_satellites);
  transformed_values[7] = static_cast<unsigned int>(bbman.gps_antenna_status);
  transformed_values[8] = static_cast<unsigned int>(bbman.failure_flags);

  return transformed_values;
}

/*
 * transform_launchready
 * Parameters: A BBManager object
 * Returns: A array of unsigned ints
 * Notes:
 *      - Follow guidelines of the LAUNCH READY bitfield schema,
 *      - Some fields don't need to undergo DLT and simply need to
 *            be recasted
 *      - The BBManager object is used to get current sensor readings
 */
unsigned int *transform_launchready(BBManager bbman)
{
  // there are 26 fields in this schema
  static unsigned int transformed_values[26];

  // STATE
  transformed_values[0] = static_cast<unsigned int>(bbman.curr_state);

  // GPS LINK QUALITY; NUM OF SATELITTES LIKED TO MODULE
  transformed_values[1] = bbman.gps_num_satellites;

  // GPS LONGITUDE COORDINATE SIGN (pos or negative)
  transformed_values[2] = (bbman.gps_long > 0) ? 1 : ((bbman.gps_long < 0) ? 0 : 0); // if > 0, then set to 1, otherwise set to 0

  // GPS LONGITUDE COORDINATES, making it positive if needed
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
  transformed_values[9] = serialize_dlt(15, -50, 350, bbman.k_vert_velocity, VERT_VELO_SPACING);

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
  transformed_values[20] = static_cast<unsigned int>(bbman.failure_flags);

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
 * Parameters: A BBManager object
 * Returns: A array of unsigned ints
 * Notes:
 *      - Follow guidelines of the LAUNCH MODE bitfield schema,
 *      - Some fields don't need to undergo DLT and simply need to
 *            be recasted
 *      - The BBManager object is used to get current sensor readings
 */
unsigned int *transform_launchmode(BBManager bbman)
{
  // there are 27 fields in this schema
  static unsigned int transformed_values[27];

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
  transformed_values[22] = static_cast<unsigned int>(bbman.failure_flags);

  // GPS SPEED MEASUREMENTS
  transformed_values[23] = serialize_dlt(10, 0, 70, bbman.gps_speed, GPS_SPEED_SPACING);

  // KF VERTICAL VELOCITY CALCULATIONS
  transformed_values[24] = serialize_dlt(15, -50, 350, bbman.k_vert_velocity, VERT_VELO_SPACING);

  // GPS SIGNAL QUALITY
  transformed_values[25] = static_cast<unsigned int>(bbman.gps_quality);

  // GPS FIX
  transformed_values[26] = static_cast<unsigned int>(bbman.gps_fix);

  return transformed_values;
}

/*
 * transform_recovery
 * Parameters: A BBManager object
 * Returns: A array of unsigned ints
 * Notes:
 *      - Follow guidelines of the RECOVERY bitfield schema,
 *      - Some fields don't need to undergo DLT and simply need to
 *            be recasted
 *      - The BBManager object is used to get current sensor readings
 */
unsigned int *transform_recovery(BBManager bbman)
{
  // there are 13 fields in this schema
  static unsigned int transformed_values[13];

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
  transformed_values[10] = static_cast<unsigned int>(bbman.failure_flags);
  transformed_values[11] = static_cast<unsigned int>(bbman.gps_quality);
  transformed_values[12] = static_cast<unsigned int>(bbman.gps_antenna_status);

  return transformed_values;
}

/*
 * untransform_poweron
 * Parameters: An array of unsigned ints that have gone through DLT
 * Returns: A struct containing the data transmitted in the POWER ON state
 * Purpose: Returns the true (lossy) sensor readings
 * Notes: Follow guidelines of the POWER ON bitfield schema
 */
powerondata untransform_poweron(unsigned int *data)
{
  powerondata tempd_po;

  tempd_po.curr_state = data[0];
  tempd_po.external_temp = deserialize_dlt(data[1], -15, EXT_TEMP_SPACING);
  tempd_po.temperature_engbay = deserialize_dlt(data[2], 0, INT_TEMP_SPACING);
  tempd_po.temperature_avbay = deserialize_dlt(data[3], 0, INT_TEMP_SPACING);
  tempd_po.gps_quality = data[4];
  tempd_po.gps_fix = data[5];
  tempd_po.gps_num_satellites = data[6];
  tempd_po.gps_antenna_status = data[7];
  tempd_po.failures = data[8];

  return tempd_po;
}

/*
 * untransform_launchready
 * Parameters: An array of unsigned ints that have gone through DLT
 * Returns: A struct containing the data transmitted in the LAUNCH READY state
 * Purpose: Returns the true (lossy) sensor readings
 * Notes: Follow guidelines of the LAUNCH READY bitfield schema
 */
launchreadydata untransform_launchready(unsigned int *data)
{
  launchreadydata tempd_lr;

  tempd_lr.curr_state = data[0];
  tempd_lr.gps_num_satellites = data[1];

  if (data[2] > 1)
  {
    tempd_lr.gps_long = data[3];
  }
  else
  {
    tempd_lr.gps_long = data[3] * -1;
  }

  if (data[4] > 1)
  {
    tempd_lr.gps_lat = data[5];
  }
  else
  {
    tempd_lr.gps_lat = data[5] * -1;
  }

  tempd_lr.gyro_x = deserialize_dlt(data[6], -1440, GYRO_XY_SPACING);
  tempd_lr.accel_y = deserialize_dlt(data[7], 0, ACCEL_XY_SPACING);
  tempd_lr.gyro_y = deserialize_dlt(data[8], -1440, GYRO_XY_SPACING);
  tempd_lr.vert_velo = deserialize_dlt(data[9], -50, VERT_VELO_SPACING);
  tempd_lr.gyro_z = deserialize_dlt(data[10], -360, GYRO_Z_SPACING);
  tempd_lr.accel_x = deserialize_dlt(data[11], 0, ACCEL_XY_SPACING);
  tempd_lr.altitude = deserialize_dlt(data[12], 0, ALTITUDE_SPACING);
  tempd_lr.gps_fix = data[13];
  tempd_lr.external_temp = deserialize_dlt(data[14], -15, EXT_TEMP_SPACING);
  tempd_lr.temperature_avbay = deserialize_dlt(data[15], 0, INT_TEMP_SPACING);
  tempd_lr.accel_z = deserialize_dlt(data[16], -30, ACCEL_Z_SPACING);
  tempd_lr.mag_x = deserialize_dlt(data[17], -5, MAG_FORCE_SPACING);
  tempd_lr.mag_y = deserialize_dlt(data[18], -5, MAG_FORCE_SPACING);
  tempd_lr.mag_z = deserialize_dlt(data[19], -5, MAG_FORCE_SPACING);
  tempd_lr.failures = data[20];
  tempd_lr.gps_speed = deserialize_dlt(data[21], 0, GPS_SPEED_SPACING);
  tempd_lr.gps_altitude = deserialize_dlt(data[22], 0, ALTITUDE_SPACING);
  tempd_lr.gps_quality = data[23];
  tempd_lr.temperature_engbay = deserialize_dlt(data[24], 0, INT_TEMP_SPACING);
  tempd_lr.gps_antenna_status = data[25];

  return tempd_lr;
}

/*
 * untransform_launchmode
 * Parameters: An array of unsigned ints that have gone through DLT
 * Returns: A struct containing the data transmitted in the LAUNCH MODE state
 * Purpose: Returns the true (lossy) sensor readings
 * Notes: Follow guidelines of the LAUNCH MODE bitfield schema
 */
launchmodedata untransform_launchmode(unsigned int *data)
{
  launchmodedata tempd_lm;

  tempd_lm.curr_state = data[0];
  tempd_lm.gps_num_satellites = data[1];

  if (data[2] > 1)
  {
    tempd_lm.gps_long = data[3];
  }
  else
  {
    tempd_lm.gps_long = data[3] * -1;
  }

  if (data[4] > 1)
  {
    tempd_lm.gps_lat = data[5];
  }
  else
  {
    tempd_lm.gps_lat = data[5] * -1;
  }

  tempd_lm.gyro_x = deserialize_dlt(data[6], -1440, GYRO_XY_SPACING);
  tempd_lm.gps_altitude = deserialize_dlt(data[7], 0, ALTITUDE_SPACING);
  tempd_lm.accel_x = deserialize_dlt(data[8], 0, ACCEL_XY_SPACING);
  tempd_lm.gyro_y = deserialize_dlt(data[9], -1440, GYRO_XY_SPACING);
  tempd_lm.timestamp = data[10];
  tempd_lm.gyro_z = deserialize_dlt(data[11], -360, GYRO_Z_SPACING);
  tempd_lm.altitude = deserialize_dlt(data[12], 0, ALTITUDE_SPACING);
  tempd_lm.gps_antenna_status = data[13];
  tempd_lm.external_temp = deserialize_dlt(data[14], -15, EXT_TEMP_SPACING);
  tempd_lm.accel_y = deserialize_dlt(data[15], 0, ACCEL_XY_SPACING);
  tempd_lm.temperature_avbay = deserialize_dlt(data[16], 0, INT_TEMP_SPACING);
  tempd_lm.temperature_engbay = deserialize_dlt(data[17], 0, INT_TEMP_SPACING);
  tempd_lm.accel_z = deserialize_dlt(data[18], -30, ACCEL_Z_SPACING);
  tempd_lm.mag_x = deserialize_dlt(data[19], -5, MAG_FORCE_SPACING);
  tempd_lm.mag_y = deserialize_dlt(data[20], -5, MAG_FORCE_SPACING);
  tempd_lm.mag_z = deserialize_dlt(data[21], -5, MAG_FORCE_SPACING);
  tempd_lm.failures = data[22];
  tempd_lm.gps_speed = deserialize_dlt(data[23], 0, GPS_SPEED_SPACING);
  tempd_lm.vert_velo = deserialize_dlt(data[24], -50, VERT_VELO_SPACING);
  tempd_lm.gps_quality = data[25];
  tempd_lm.gps_fix = data[26];

  return tempd_lm;
}

/*
 * untransform_recovery
 * Parameters: An array of unsigned ints that have gone through DLT
 * Returns: A struct containing the data transmitted in the RECOVERY state
 * Purpose: Returns the true (lossy) sensor readings
 * Notes: Follow guidelines of the RECOVERY bitfield schema
 */
recoverydata untransform_recovery(unsigned int *data)
{
  recoverydata tempd_r;

  tempd_r.curr_state = data[0];
  tempd_r.gps_num_satellites = data[1];

  if (data[2] > 1)
  {
    tempd_r.gps_long = data[3];
  }
  else
  {
    tempd_r.gps_long = data[3] * -1;
  }

  if (data[4] > 1)
  {
    tempd_r.gps_lat = data[5];
  }
  else
  {
    tempd_r.gps_lat = data[5] * -1;
  }

  tempd_r.external_temp = deserialize_dlt(data[6], -15, EXT_TEMP_SPACING);
  tempd_r.temperature_engbay = deserialize_dlt(data[7], 0, INT_TEMP_SPACING);
  tempd_r.temperature_avbay = deserialize_dlt(data[8], 0, INT_TEMP_SPACING);
  tempd_r.gps_fix = data[9];
  tempd_r.failures = data[10];
  tempd_r.gps_quality = data[11];
  tempd_r.gps_antenna_status = data[12];

  return tempd_r;
}