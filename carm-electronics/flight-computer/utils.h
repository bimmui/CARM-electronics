/**************************************************************
 *
 *                     utils.h
 *
 *     Author(s):  Daniel Opara,
 *     Date:       1/9/2024
 *
 *     Overview: A collection of utility funtions
 *
 *
 **************************************************************/

#include <math.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <inttypes.h>
#include "def.h"

/*
 * convert_to_decimaldegrees
 * Parameters: Coordinate value in the form of degrees and decimal minutes and the cardinal direction
 * Purpose: Converts GPS coordinates from degrees and decimal minutes format to decimal degrees format
 * Returns: Nothing
 * Notes: The GPS module outputs coordinates in degrees and decimal minutes which is harder to
 *          to interpret than decimal degrees
 */
float convert_to_decimaldegrees(float coordinate, char direction)
{
    // extracting the degrees and minutes from the coordinate value

    int degrees = (int)(coordinate / 100);   // first two digits represent degrees
    float minutes = fmod(coordinate, 100.0); // remaining part are minutes

    float decimal_degrees = (degrees + (minutes / 60.0));

    // check the direction (N/S for latitude, E/W for longitude)
    if (direction == 'S' || direction == 'W')
    {
        decimal_degrees = -decimal_degrees; // make the result negative for southern or western hemispheres
    }

    decimal_degrees = round(decimal_degrees * 10000.0) / 10000.0;
    return decimal_degrees;
}

/*
 * calculateAverageAcceleration
 * Parameters: The acceleration in the x, y, and z direction
 * Purpose: Calculates the average acceleration of the rocket
 * Returns: Nothing
 */
float calculate_average_acceleration(float x_accel, float y_accel, float z_accel)
{
    float sum_squared_coordinates = (x_accel * x_accel) + (y_accel * y_accel) + (z_accel * z_accel);
    return sqrt(sum_squared_coordinates);
}

/*
 * convert_knots_to_mps
 * Parameters: The speed in knots
 * Purpose: Converts the unit of speed from knots to meters per second (m/s)
 * Returns: Nothing
 */
float convert_knots_to_mps(float knots)
{
    return knots * 0.514444;
}

void transmit(RH_RF95 rf96, uint64_t packet[], uint8_t packet_len)
{
    uint64_t temp_packet[packet_len];
    delay(10);
    rf96.send((uint8_t *)packet, sizeof(temp_packet));
    delay(10);
    rf96.waitPacketSent();
}

uint64_t *receive(RH_RF95 rf96, uint8_t packet_len)
{
    if (rf96.available())
    {
        // Should be a message for us now
        static uint64_t buf[packet_len];
        uint8_t size = sizeof(buf);

        if (rf96.recv((uint8_t *)buf, &size))
        {
            return buf;
        }
        else
        {
            static uint64_t fail[1] = {0};
            return fail;
        }
    }
}