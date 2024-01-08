/**************************************************************
 *
 *                     carm_computer.ino
 *
 *     Author(s):  Daniel Opara,
 *     Date:       1/6/2023
 *
 *     Overview: Driver code for the rocket computer. Gets readings from the sensors
 *                  connected to it and transmits the data to the ground station.
 *
 *
 **************************************************************/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS1.h> // IMU module
#include "Adafruit_BMP3XX.h"  // BMP module
#include "Adafruit_MCP9808.h" // Temp sensor module
#include "sensor_setup.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Defining digital pins, constants, and sensor objects
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// BMP_SCK == SCL
#define BMP_SCK 18
// BMP_CS == CS
#define BMP_CS 17
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();
Adafruit_BMP3XX bmp;
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

void setup()
{
    Serial.begin(115200);
    setupSensorIMU(lsm);
    setupSensorBMP(bmp);
    setupSensorTemp(tempsensor);
}

void loop()
{
    tempsensor.wake();
    // get readings from all the sensors
    lsm.read();
    sensors_event_t a, m, g, temp;
    if (!bmp.performReading())
    {
        Serial.println("Failed to perform reading :(");
        return;
    }

    // print out readings
    Serial.print("Temperature Sensor = ");
    Serial.println(tempsensor.readTempC());

    Serial.print("Temperature = ");
    Serial.print(bmp.temperature);
    Serial.println(" *C");

    Serial.print("Pressure = ");
    Serial.print(bmp.pressure / 100.0);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.println();
    lsm.getEvent(&a, &m, &g, &temp);
    Serial.print("Accel X: ");
    Serial.print(a.acceleration.x);
    Serial.print(" Y: ");
    Serial.print(a.acceleration.y);
    Serial.print(" Z: ");
    Serial.println(a.acceleration.z);

    Serial.print("Mag X: ");
    Serial.print(m.magnetic.x);
    Serial.print(" Y: ");
    Serial.print(m.magnetic.y);
    Serial.print(" Z: ");
    Serial.println(m.magnetic.z);

    Serial.print("Gyro X: ");
    Serial.print(g.gyro.x);
    Serial.print(" Y: ");
    Serial.print(g.gyro.y);
    Serial.print(" Z: ");
    Serial.println(g.gyro.z);
    delay(2000);
}