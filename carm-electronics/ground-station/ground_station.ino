#include <RFM69.h>
#include <SQLite.h>

RFM69 radio;
SQLite db("rocket_data.db");

void setup() {
    // Initialize radio and database
    radio.init();
    db.open();
    db.createTable("sensor_data", "time REAL, temperature_avbay REAL, ...");
    Serial.begin(9600); // Initialize serial communication
}

void loop() {
    SensorData data;
    if (radio.receive(data, sizeof(SensorData))) {
        // Store data in SQL database
        db.insert("sensor_data", data);

        // Pass data to serial port
        Serial.write((uint8_t*)&data, sizeof(SensorData));
    }
}
