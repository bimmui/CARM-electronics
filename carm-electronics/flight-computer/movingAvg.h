/* moving_avg.h
 * Noah Stiegler
 * 2/18/24
 *
 * A class for keeping track of a moving average of sensor readings within
 * a given window (number of values to average over). Floating point values
 * can be added to the average and the average can be read
 *
 */

#ifndef MOVING_AVG_H
#define MOVING_AVG_H

#include <cppQueue.h>
#include <stdint.h>
#include "def.h"

class MovingAvg:
{
public:
    MovingAvg(unsigned moving_avg_window);
    void take_new_measurement(float val); //
    float get_average();                  // returns running sum

private:
    cppQueue vals;     // All values in moving average
    float sum;         // Sum of values in vals
    unsigned capacity; // Number of values to average over
    uint16_t used;     // How many values are in the queue (before it's full)
};

#endif