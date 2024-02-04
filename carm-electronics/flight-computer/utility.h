/**************************************************************
 *
 *                     utility.h
 *
 *     Author(s):  Daniel Opara,
 *     Date:       1/9/2023
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
 * mps_to_mph
 * Parameters: Speed in meters per second
 * Purpose: Converts speed from meters per second to miles per hour
 * Returns: Speed in miles per hour
 * Notes: None
 */
template <typename T>
T mps_to_mph(T mps)
{
    return mps * 2.236936;
}

