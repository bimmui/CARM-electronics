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
  unsigned int serialized_data = floor(((reading - n_min) / m_spacing) - 1);
  return serialized_data;
}

float deserialize_dlt(unsigned int serialized, int n_min, float m_spacing)
{
  float deserialized_data = (((serialized + 1) * m_spacing) + n_min);
  return deserialized_data;
}

unsigned int *transform_poweron(BBManager bbman)
{
  unsigned int transformed_values[8];

  transformed_values[0] = static_cast<unsigned int>(bbman.curr_state);
  transformed_values[1] = serialize_dlt(11, -15, 125, bbman.external_temp);
  transformed_values[2] = serialize_dlt(11, 0, 127, bbman.temperature_engbay);
  transformed_values[3] = serialize_dlt(11, 0, 127, bbman.temperature_avbay);
  transformed_values[4] = bbman.gps_quality;
  transformed_values[5] = bbman.gps_num_satellites;
  transformed_values[6] = bbman.gps_antenna_status;
  transformed_values[7] = static_cast<unsigned int>(bbman.errors_bit_mask);

  return transformed_values;
}