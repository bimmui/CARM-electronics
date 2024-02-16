/**************************************************************
 *
 *                     utility.h
 *
 *     Author(s):  Daniel Opara,
 *     Date:       1/9/2024
 *
 *     Overview: A collection of utility funtions
 *
 *
 **************************************************************/

#include <math.h>

/*
 * convertToDecimalDegrees
 * Parameters: Coordinate value in the form of degrees and decimal minutes and the cardinal direction
 * Purpose: Converts GPS coordinates from degrees and decimal minutes format to decimal degrees format
 * Returns: Nothing
 * Notes: The GPS module outputs the
 */
double convertToDecimalDegrees(double coordinate, char direction)
{
    // extracting the degrees and minutes from the coordinate value

    int degrees = (int)(coordinate / 100);    // first two digits represent degrees
    double minutes = fmod(coordinate, 100.0); // remaining part are minutes

    double decimal_degrees = (degrees + (minutes / 60.0));

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
 * Notes:
 */
float calculateAverageAcceleration(float x_accel, float y_accel, float z_accel)
{
    float sum_squared_coordinates = (x_accel * x_accel) + (y_accel * y_accel) + (z_accel * z_accel);
    return sqrt(sum_squared_coordinates);
}