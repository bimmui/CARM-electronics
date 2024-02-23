/**************************************************************
 *
 *                     MovingAvg.cpp
 *
 *     Author(s):  Noah Stiegler, Daniel Opara
 *     Date:       2/18/2024
 *
 *     Overview: The implementation for MovingAvg. Utilizes cppQueue by SMFSW
 *                  for storing values, operates on float values. For more info,
 *                  please visit https://github.com/SMFSW/Queue
 *
 *
 **************************************************************/

#include "MovingAvg.h"
#include "def.h"

/*
 * MovingAvg constructor
 * Parameters: None
 * Purpose: Initializes a queue using the cppQueue library and sets the sum to zero
 * Notes: None
 *
 */
MovingAvg::MovingAvg() : vals(sizeof(float), QUEUE_MAX_LENGTH, IMPLEMENTATION, false)
{
    sum = 0;
    partial_sum = 0;
    partial_sum_nums = 0;
}

/*
 * MovingAvg destructor
 * Parameters: None
 * Purpose: Cleans up mem of MovingAvg objects
 * Notes: None
 *
 */
MovingAvg::~MovingAvg()
{
    // some instructions
}

/*
 * init
 * Parameters: An array of float values and the length of the subset of values that
 *                will be used to calculate a partial average.
 * Purpose: Fills the cppQueue with QUEUE_MAX_LENGTH readings and sets proper values
 *              for sum and partial sum
 * Notes: Must call this function before calling anything else of this class, you're
 *          more likely to get undefined behavior this way
 *
 */
MovingAvg::init(float sensor_readings[QUEUE_MAX_LENGTH], int partial_sum_count)
{
    sum = 0;
    for (int i = 0; i < QUEUE_MAX_LENGTH; i++)
    {
        vals.push(&(sensor_readings[i]));
        sum += sensor_readings[i];
    }
    for (int i = 0; i < partial_sum_count; i++)
    {
        partial_sum += sensor_readings[i];
    }
    partial_sum_nums = partial_sum_count
}

/*
 * add_new_measurement
 * Parameters: A float value
 * Purpose: Adds a value to the queue and adjusts sum accordingly
 * Notes: If the queue is full, the oldest value is popped to make space for
 *          the new value. The sum is adjusted accordingly.
 *
 */
void MovingAvg::add_new_measurement(float new_val)
{
    if (vals.isFull())
    {
        // Pop oldest value and adjust sum for new value being added
        float popped_val;
        vals.pop(&popped_val);
        sum -= popped_val;
        vals.push(&new_val);
        sum += new_val;
    }
    else
    {
        // Push val onto queue and add it to the sum
        vals.push(&new_val);
        sum += new_val;
    }
}

/*
 * get_average
 * Parameters: None
 * Purpose: Calculates the average of the values in the queue
 * Notes: None
 *
 */
float MovingAvg::get_average()
{
    return sum / (1.0 * QUEUE_MAX_LENGTH);
}

/*
 * get_partial_average
 * Parameters: An integer that represents the number of
 * Purpose: Calculates the average of the values in the queue
 * Notes: None
 *
 */
float MovingAvg::get_partial_average()
{
    return sum / (1.0 * QUEUE_MAX_LENGTH);
}