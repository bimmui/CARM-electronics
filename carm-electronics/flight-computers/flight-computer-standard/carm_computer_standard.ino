/**************************************************************
 *
 *                     carm_computer_standard.ino
 *
 *     Author(s):  Daniel Opara,
 *     Date:       1/6/2023
 *
 *     Overview: Driver code for the rocket computer. Gets readings from the sensors
 *                  connected to it and writes it to an SD card.
 *
 *
 **************************************************************/

#include <Wire.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS1.h> // IMU module
#include "Adafruit_BMP3XX.h"  // BMP module
#include "Adafruit_MCP9808.h" // Temp sensor module
#include "bb_setup.h"

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
#define CHIPSELECT 13

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Defining sensor objects and structs
// - - - - - - - - - - - - - - - - - - - - - - - - - - -

Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();               // imu
Adafruit_BMP3XX bmp;                                     // barometric pressure sensor
Adafruit_MCP9808 tempsensor_avbay = Adafruit_MCP9808();  // avionics bay thermocouple
Adafruit_MCP9808 tempsensor_engbay = Adafruit_MCP9808(); // engine bay thermocouple
File launch_data;                                        // file object for writing data

struct SensorData
{
    float time;
    float temperature_av;
    float temperature_eng;
    float bmp_temp;
    float pressure;
    float altitude;
    float accel_x;
    float accel_y;
    float accel_z;
    float mag_x;
    float mag_y;
    float mag_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
};

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

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Function contracts
// - - - - - - - - - - - - - - - - - - - - - - - - - - -

void setup()
{
    Serial.begin(115200); // serial used for testing
    bool imu_setup = setupSensorIMU(lsm);
    bool bmp_setup = setupSensorBMP(bmp);
    bool temp_setup1 = setupSensorTemp(tempsensor_avbay, 0x18);
    bool temp_setup2 = setupSensorTemp(tempsensor_engbay, 0x19);

    // pinMode(CHIPSELECT, OUTPUT);
    bool sd_setup = setupSD(CHIPSELECT);
    // TODO: Add a case where the SD card module setup func returns false
    launch_data = SD.open("initialzing.txt", FILE_WRITE);
    if (launch_data)
    {
        if (imu_setup)
        {
            launch_data.println("IMU successfully set up");
        }
        else
        {
            launch_data.println("IMU unsuccessfully set up");
        }

        if (bmp_setup)
        {
            launch_data.println("BMP successfully set up");
        }
        else
        {
            launch_data.println("BMP unsuccessfully set up");
        }

        if (temp_setup1)
        {
            launch_data.println("Avionics bay temp. sensor successfully set up");
        }
        else
        {
            launch_data.println("Avionics bay temp. sensor unsuccessfully set up");
        }

        if (temp_setup2)
        {
            launch_data.println("Engine bay temp. sensor successfully set up");
        }
        else
        {
            launch_data.println("Engine bay temp. sensor unsuccessfully set up");
        }

        launch_data.close();
    }
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