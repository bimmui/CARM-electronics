/**************************************************************
 *
 *                     micro_v_millimulti.ino
 *
 *     Author(s):  Daniel Opara
 *     Date:       4/15/2024
 *
 *     Overview: Uses Profiler library to check how long term, consisitant
 *                use of micros() and millis() * 1000 compare to each other. Only doing
 *                this because I read on some blog that micros() takes longer than
 *                millis() and I wanted to quantify this
 *
 *
 *
 **************************************************************/

#include <Profiler.h>

#define DEBUG_LED 13

void prof_micros()
{
    profiler_t profiler;
    for (int i = 0; i < 100000000; i++)
    {
        unsigned micro = micros();
    }
}

void prof_millismulti()
{
    profiler_t profiler;
    for (int i = 0; i < 100000000; i++)
    {
        unsigned pusedo_micro = millis() * 1000;
    }
}

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    Serial.println("Performing millismultiply profiling...");
    prof_millismulti();

    Serial.println("Performing micros profiling...");
    prof_micros();
}

void loop() {}