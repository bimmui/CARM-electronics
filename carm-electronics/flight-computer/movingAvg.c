/* moving_avg.c
 * Noah Stiegler
 * 2/18/24
 *
 * A class for keeping track of a moving average of sensor readings within
 * a given window (number of values to average over). Floating point values
 * can be added to the average and the average can be read.
 *
 * Implemented with a cppQueue. The queue can have
 *
 */

#include "movingAvg.h"

/*
 * movingAvg constructor
 * Parameters: The number of values it averages over - how many it holds before
 *             it starts forgetting past values
 * Purpose: Creates an empty moving average class which averages over the given
 *          "window" if it holds that many values otherwise averages over the values
 *          it holds
 * Notes: Creates an EMPTY moving average which has no value defined (returns 0)
 *
 */
movingAvg(unsigned moving_avg_window)
{
    sum = 0;                      // If it's empty we'll call it a 0 average (0/0 = 0)
    capacity = moving_avg_window; // Number of values to average over
    vals = cppQueue(sizeof(float), moving_avg_window, IMPLEMENTATION)
        used = 0; // How many values are in the queue (before it's full)
}

void take_new_measurement(float val)
{
    if (isFull())
    {
        // Pop end of queue and subtract that number from sum
        // Push val onto queue and add it to sum
    }
    else
    {
        // Push val onto queue and add it to the sum
        // Increase used by 1
    }
}
float get_average()
{
    return sum / (1.0 * capacity); // return average, making sure capacity is float to ensure float division
}

// See if queue is full or values still need to be added
bool isFull()
{
    return used == capacity;
}