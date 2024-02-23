/**************************************************************
 *
 *                     MovingAvg.h
 *
 *     Author(s):  Noah Stiegler, Daniel Opara
 *     Date:       2/18/2024
 *
 *     Overview: A class for keeping track of a moving average of sensor readings within
 *                  a given window (number of values to average over).
 *
 *
 **************************************************************/

#ifndef MOVING_AVG_H
#define MOVING_AVG_H

#include <cppQueue.h>
#include <stdint.h>
#include "def.h"

class MovingAvg
{
public:
    MovingAvg(float sensor_readings[QUEUE_MAX_LENGTH]);
    void add_new_measurement(float new_val);
    float get_average();

private:
    cppQueue vals;
    float sum;
};

#endif