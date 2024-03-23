#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <iostream>
#include <inttypes.h>
using namespace std;

#include "../../carm-electronics/flight-computer/bitpack.c"

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

// Note: this bitpacking library was extensively tested prior to its inclusion to this project,
//          so this type of testing will be a bit lax, especially since we will be working with
//          unsigned number exclusively
TEST_CASE("General bitpacking tests; ensuring exact values are retrieved as they were before packing")
{
    uint64_t test_word = 0;

    uint64_t test = 22;
    uint64_t bitpacked_word = Bitpack_newu(test_word, 5, 0, 22);
    unsigned int basic_test = Bitpack_getu(bitpacked_word, 5, 0);

    CHECK(basic_test == 22);
}

TEST_CASE("Testing bitpacking with DLT values")
{
    SUBCASE("Packing and unpacking serialized data; we use possible values obtained from external temperature's range")
    {
        uint64_t test_word1 = 0;
        uint64_t test_word2 = 0;
        uint64_t test_word3 = 0;
        uint64_t test_word4 = 0;
        uint64_t test_word5 = 0;
        uint64_t test_word6 = 0;
        uint64_t test_word7 = 0;
        uint64_t test_word8 = 0;
        uint64_t test_word9 = 0;
        uint64_t test_word10 = 0;

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

        unsigned int unpacked_min_val = Bitpack_getu(Bitpack_newu(test_word1, 14, 4, min_val), 14, 4);
        unsigned int unpacked_max_val = Bitpack_getu(Bitpack_newu(test_word2, 14, 4, max_val), 14, 4);
        unsigned int unpacked_neg_val = Bitpack_getu(Bitpack_newu(test_word3, 14, 4, neg_val), 14, 4);
        unsigned int unpacked_neg_val_w_decimal = Bitpack_getu(Bitpack_newu(test_word4, 14, 4, neg_val_w_decimal), 14, 4);
        unsigned int unpacked_pos_val_w_decimal = Bitpack_getu(Bitpack_newu(test_word5, 14, 4, pos_val_w_decimal), 14, 4);
        unsigned int unpacked_just_below_min_range = Bitpack_getu(Bitpack_newu(test_word6, 14, 4, just_below_min_range), 14, 4);
        unsigned int unpacked_below_min_range = Bitpack_getu(Bitpack_newu(test_word7, 14, 4, below_min_range), 14, 4);
        unsigned int unpacked_just_above_max_range = Bitpack_getu(Bitpack_newu(test_word8, 14, 4, just_above_max_range), 14, 4);
        unsigned int unpacked_above_max_range = Bitpack_getu(Bitpack_newu(test_word9, 14, 4, above_max_range), 14, 4);
        unsigned int unpacked_normal_case = Bitpack_getu(Bitpack_newu(test_word10, 14, 4, normal_case), 14, 4);

        CHECK(unpacked_min_val == 0);
        CHECK(unpacked_max_val == 2047);
        CHECK(unpacked_neg_val == 146);
        CHECK(unpacked_neg_val_w_decimal == 123);
        CHECK(unpacked_pos_val_w_decimal == 686);
        CHECK(unpacked_just_below_min_range == -1); // this should fail
        CHECK(unpacked_below_min_range == -74);     // this should fail
        CHECK(unpacked_just_above_max_range == 2048);
        CHECK(unpacked_above_max_range == 2412);
        CHECK(unpacked_normal_case == 592);
    }
}