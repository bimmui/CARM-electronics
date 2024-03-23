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

/*
 * calculateCurrVelocity
 * Parameters: The acceleration in the x, y, and z direction
 * Purpose: Calculates the average acceleration of the rocket
 * Returns: Nothing
 * Notes:
 */
// float calculateCurrVelocity(float prev_velocity, float y_accel, float z_accel)
// {
// }

/*
 * calculateAverageAcceleration
 * Parameters: Pointers to the queues used for calculating the moving average of
 *                 velocity, altitude, and acceleration
 * Purpose: Fills up the queues with values before starting the main control loop
 * Returns: Nothing
 * Notes: The queues need to be filled up to prevent any undefined behavior when doing
 *              calculations with empty indicies
 */
// void setupMovingAvgArrs(cppQueue velocity_arr, cppQueue altitude_arr, cppQueue accel_arr,
//                         Adafruit_LSM9DS1 &lsm_obj, Adafruit_BMP3XX &bmp_obj, )
// {
//     // Take QUEUE_MAX_LENGTH readings from each sensor and store in their
//     // moving average queues
//     for (int i = 0; i < QUEUE_MAX_LENGTH; i++)
//     {
//         //
//     }
// }