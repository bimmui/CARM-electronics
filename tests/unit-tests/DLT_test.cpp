#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <iostream>
using namespace std;

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
    float deserialized_data = (static_cast<float>(serialized * m_spacing) + n_min);
    return deserialized_data;
}

TEST_CASE("We reference the calculations done in the Desmos notebook linked in the CARM Transmission Protocol paper for checking if values are correct.") {}

TEST_CASE("Testing Discrete Lossy Transform with the ranges of different values")
{
    SUBCASE("Serialization with external temperature")
    {
        unsigned int min_val = serialize_dlt(11, -15, 125, -15.0, EXT_TEMP_SPACING);
        unsigned int max_val = serialize_dlt(11, -15, 125, 125.0, EXT_TEMP_SPACING);
        unsigned int neg_val = serialize_dlt(11, -15, 125, -5.0, EXT_TEMP_SPACING);
        unsigned int neg_val_w_decimal = serialize_dlt(11, -15, 125, -6.529249272520528522, EXT_TEMP_SPACING);
        unsigned int pos_val_w_decimal = serialize_dlt(11, -15, 125, 31.9418419481819818198, EXT_TEMP_SPACING);
        unsigned int just_below_min_range = serialize_dlt(11, -15, 125, -15.0001, EXT_TEMP_SPACING);
        unsigned int below_min_range = serialize_dlt(11, -15, 125, -20.0, EXT_TEMP_SPACING);
        unsigned int just_above_max_range = serialize_dlt(11, -15, 125, 125.1, EXT_TEMP_SPACING);
        unsigned int above_max_range = serialize_dlt(11, -15, 125, 150.0, EXT_TEMP_SPACING);
        unsigned int normal_case = serialize_dlt(11, -15, 125, 25.5, EXT_TEMP_SPACING);

        CHECK(min_val == 0);
        CHECK(max_val == 2047);
        CHECK(neg_val == 146);
        CHECK(neg_val_w_decimal == 123);
        CHECK(pos_val_w_decimal == 686);
        CHECK(just_below_min_range == -1);
        CHECK(below_min_range == -74);
        CHECK(just_above_max_range == 2048);
        CHECK(above_max_range == 2412);
        CHECK(normal_case == 592);
    }
    SUBCASE("Deserialization with external temperature")
    {
        float deserial_min_val = deserialize_dlt(serialize_dlt(11, -15, 125, -15.0, EXT_TEMP_SPACING), -15, EXT_TEMP_SPACING);
        float deserial_max_val = deserialize_dlt(serialize_dlt(11, -15, 125, 125.0, EXT_TEMP_SPACING), -15, EXT_TEMP_SPACING);
        float deserial_neg_val = deserialize_dlt(serialize_dlt(11, -15, 125, -5.0, EXT_TEMP_SPACING), -15, EXT_TEMP_SPACING);
        float deserial_neg_val_w_decimal = deserialize_dlt(serialize_dlt(11, -15, 125, -6.529249272520528522, EXT_TEMP_SPACING), -15, EXT_TEMP_SPACING);
        float deserial_pos_val_w_decimal = deserialize_dlt(serialize_dlt(11, -15, 125, 31.9418419481819818198, EXT_TEMP_SPACING), -15, EXT_TEMP_SPACING);
        float deserial_just_below_min_range = deserialize_dlt(serialize_dlt(11, -15, 125, -15.0001, EXT_TEMP_SPACING), -15, EXT_TEMP_SPACING);
        float deserial_below_min_range = deserialize_dlt(serialize_dlt(11, -15, 125, -20.0, EXT_TEMP_SPACING), -15, EXT_TEMP_SPACING);
        float deserial_just_above_max_range = deserialize_dlt(serialize_dlt(11, -15, 125, 125.1, EXT_TEMP_SPACING), -15, EXT_TEMP_SPACING);
        float deserial_above_max_range = deserialize_dlt(serialize_dlt(11, -15, 125, 150.0, EXT_TEMP_SPACING), -15, EXT_TEMP_SPACING);
        float deserial_normal_case = deserialize_dlt(serialize_dlt(11, -15, 125, 25.5, EXT_TEMP_SPACING), -15, EXT_TEMP_SPACING);

        CHECK(deserial_min_val == -15);
        CHECK(deserial_max_val == 125);
        CHECK(deserial_neg_val >= -5.02);
        CHECK(deserial_neg_val <= -5.00);
        CHECK(deserial_neg_val_w_decimal >= -6.588);
        CHECK(deserial_neg_val_w_decimal <= -6.580);
        CHECK(deserial_pos_val_w_decimal >= 31.91);
        CHECK(deserial_pos_val_w_decimal <= 31.92);
        CHECK(deserial_just_below_min_range >= -15.07);
        CHECK(deserial_just_below_min_range <= -15.0);
        CHECK(deserial_below_min_range >= -20.7);
        CHECK(deserial_below_min_range >= -20);
        CHECK(deserial_just_above_max_range >= 125.0);
        CHECK(deserial_just_above_max_range >= 125.07);
        CHECK(deserial_above_max_range >= 149.9);
        CHECK(deserial_above_max_range <= 149.97);
        CHECK(deserial_normal_case >= 25.0);
        CHECK(deserial_normal_case <= 25.7);
    }
}