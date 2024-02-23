#include <cppQueue.h>
#include "def.h"
#include "MovingAvg.h"

void setup()
{
    Serial.begin(9600);
    while (!Serial)
        ; // waits for serial terminal to be open, necessary in newer arduino boards.

    float readings[QUEUE_MAX_LENGTH];
    for (int i = 0; i < QUEUE_MAX_LENGTH; i++)
    {
        // read sensors
        readings[i] = i;
    }
    MovingAvg test = MovingAvg(readings);
    Serial.println(test.get_average());
    test.add_new_measurement(10.0);
    Serial.println(test.get_average());

    for (int i = 0; i < QUEUE_MAX_LENGTH; i++)
    {
        test.add_new_measurement(10.0 * i);
        Serial.println(test.get_average());
    }
}

void loop()
{
}