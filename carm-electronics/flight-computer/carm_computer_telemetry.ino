/**************************************************************
 *
 *                     carm_computer_telemetry.ino
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

// Ejection Charge Pin #1 (J1)
#define J1 24
// Ejection Charge Pin #2 (J2)
#define J2 23
// BMP_SCK == SCL
#define BMP_SCK 18
// BMP_CS == CS
#define BMP_CS 17
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();
Adafruit_BMP3XX bmp;
Adafruit_MCP9808 tempsensor_avbay = Adafruit_MCP9808();  // avionics bay thermocouple
Adafruit_MCP9808 tempsensor_engbay = Adafruit_MCP9808(); // engine bay thermocouple

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Defining Rocket States
// - - - - - - - - - - - - - - - - - - - - - - - - - - -

// NOT TRANSMITTING MUCH FOR THESE GUYS
// RECIEVING POWER
bool POWER_ON;
// SENSOR VALUES HAVE STABILIZED
bool LAUNCH_READY;

// WE ARE ACTUALLY TRANSMITTING HERE
// INDICATORS FROM SENSOR TELL US WE ARE IN THIS MODE
bool LAUNCH_MODE;
// TRANSITION FROM BURNOUT TO COAST PHASE IS WHEN JERK == 0 AND ACCEL IS NEGATIVE
bool POWERED_FLIGHT_PHASE;
bool COAST_PHASE;

bool APOGEE_PHASE; // determined by barometric altitude
bool DROGUE_DEPLOYMENT;
bool MAIN_DEPLOYMENT;
bool RECOVERY_PHASE;

void setup()
{
    Serial.begin(115200);
    bool imu_setup = setupSensorIMU(lsm);
    bool bmp_setup = setupSensorBMP(bmp);
    bool temp_setup1 = setupSensorTemp(tempsensor_avbay, 0x18);
    bool temp_setup2 = setupSensorTemp(tempsensor_engbay, 0x19);
    tempsensor.wake();
}

void loop()
{
    // get readings from all the sensors
    lsm.read();
    sensors_event_t a, m, g, temp;
    if (!bmp.performReading())
    {
        Serial.println("Failed to perform reading :(");
        return;
    }

    // print out readings
    Serial.print("Temperature Sensor #1 = ");
    Serial.println(tempsensor_avbay.readTempC());

    Serial.print("Temperature Sensor #2 = ");
    Serial.println(tempsensor_engbay.readTempC());

    Serial.print("Temperature = ");
    Serial.print(bmp.temperature);
    Serial.println(" *C");

    Serial.print("Pressure = ");
    Serial.print(bmp.pressure / 100.0);
    Serial.println(" kPa");

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