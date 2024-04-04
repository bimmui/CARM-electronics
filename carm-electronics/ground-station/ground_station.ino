/**************************************************************
 *
 *                     carm_computer_launch1.ino
 *
 *     Author(s):  Daniel Opara
 *     Date:       1/6/2024
 *
 *     Overview: Driver code for the rocket computer. Gets readings from the sensors
 *                  connected to it and writes it to an SD card.
 *
 *
 **************************************************************/

#include <RH_RF95.h>

RFM69 radio;

void setup()
{
    // Initialize radio and database
    radio.init();
    db.open();
    db.createTable("sensor_data", "time REAL, temperature_avbay REAL, ...");
    Serial.begin(9600); // Initialize serial communication
}

void loop()
{
    SensorData data;
    if (radio.receive(data, sizeof(SensorData)))
    {
        // Store data in SQL database
        db.insert("sensor_data", data);

        // Pass data to serial port
        Serial.write((uint8_t *)&data, sizeof(SensorData));
    }
}
