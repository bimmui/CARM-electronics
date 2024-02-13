/**************************************************************
 *
 *                     carm_computer_standard.ino
 *
 *     Author(s):  Daniel Opara, Kenny Chua
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
#include <Adafruit_GPS.h>     // GPS module
#include "bb_setup.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Defining digital pins, constants, and sensor objects
// - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define RF95_FREQ 433.0

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
#define GPSSerial Serial1

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Defining sensor objects and structs
// - - - - - - - - - - - - - - - - - - - - - - - - - - -

Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();               // imu
Adafruit_BMP3XX bmp;                                     // barometric pressure sensor
Adafruit_MCP9808 tempsensor_avbay = Adafruit_MCP9808();  // avionics bay thermocouple
Adafruit_MCP9808 tempsensor_engbay = Adafruit_MCP9808(); // engine bay thermocouple
File launch_data;                                        // file object for writing data
File error_data;                                         // file object for errors
Adafruit_GPS GPS(&GPSSerial);                            // hardware GPS object

struct SensorData
{
    double time;
    float temperature_avbay;
    float temperature_engbay;
    float altimeter_temp;
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
    float gps_lat;
    float gps_long;
    int state;
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
void readSensors(SensorData &sensor_data, Adafruit_LSM9DS1 &lsm_obj,
                 Adafruit_BMP3XX &bmp_obj, Adafruit_MCP9808 &tempsensor_obj1,
                 Adafruit_MCP9808 &tempsensor_obj2);
void storeSensorData(SensorData &sensor_data);
// TODO: Complete this function. Also test if constantly
//         switching devices is a thing that can be done
void switchSPIDevice();
// TODO: Add function that switches states.

void setup()
{
    Serial.begin(115200); // serial used for testing
    bool imu_setup = setupSensorIMU(lsm);
    bool bmp_setup = setupSensorBMP(bmp);
    bool temp_setup1 = setupSensorTemp(tempsensor_avbay, 0x18);
    bool temp_setup2 = setupSensorTemp(tempsensor_engbay, 0x19);
    bool gps_setup = setupGPS(GPS);

    // Switch the spi device before setting up the SD card module

    bool sd_setup = setupSD(CHIPSELECT);
    // TODO: Add a case where the SD card module setup func returns false
    File init_data = SD.open("initializing.txt", FILE_WRITE);
    if (init_data)
    {
        if (imu_setup)
        {
            init_data.println("IMU successfully set up");
        }
        else
        {
            init_data.println("IMU unsuccessfully set up");
        }

        if (bmp_setup)
        {
            init_data.println("BMP successfully set up");
        }
        else
        {
            init_data.println("BMP unsuccessfully set up");
        }

        if (temp_setup1)
        {
            init_data.println("Avionics bay temp. sensor successfully set up");
        }
        else
        {
            init_data.println("Avionics bay temp. sensor unsuccessfully set up");
        }

        if (temp_setup2)
        {
            init_data.println("Engine bay temp. sensor successfully set up");
        }
        else
        {
            init_data.println("Engine bay temp. sensor unsuccessfully set up");
        }
        if (gps_setup)
        {
            init_data.println("GPS module successfully set up");
        }
        else
        {
            init_data.println("GPS module unsuccessfully set up");
        }

        init_data.close();
    }
}

void loop()
{
    SensorData data = {0};
    readSensors(data, lsm, bmp, tempsensor_avbay, tempsensor_engbay);
    storeSensorData(data);
}

void readSensors(SensorData &sensor_data, Adafruit_LSM9DS1 &lsm_obj,
                 Adafruit_BMP3XX &bmp_obj, Adafruit_MCP9808 &tempsensor_obj1,
                 Adafruit_MCP9808 &tempsensor_obj2)
{
    // get readings from all the sensors that store them in the sensor objects themselves
    lsm_obj.read();
    sensors_event_t a, m, g, temp;
    lsm_obj.getEvent(&a, &m, &g, &temp);
    if (!bmp_obj.performReading())
    {
        // TODO: Do something that informs the struct that the reading didnt occur
    }

    // store the sensor readings in the struct
    sensor_data.time = millis();                                  // TODO: change this to be T+ time
    sensor_data.temperature_avbay = tempsensor_obj1.readTempC();  // TODO: add error handling
    sensor_data.temperature_engbay = tempsensor_obj2.readTempC(); // TODO: add error handling
    sensor_data.pressure = bmp_obj.pressure / 100.0;
    sensor_data.altitude = bmp_obj.readAltitude(SEALEVELPRESSURE_HPA); // TODO: add error handling
    sensor_data.accel_x = a.acceleration.x;
    sensor_data.accel_y = a.acceleration.y;
    sensor_data.accel_z = a.acceleration.z;
    sensor_data.mag_x = m.magnetic.x;
    sensor_data.mag_y = m.magnetic.y;
    sensor_data.mag_z = m.magnetic.z;
    sensor_data.gyro_x = g.gyro.x;
    sensor_data.gyro_y = g.gyro.y;
    sensor_data.gyro_z = g.gyro.z;
}

void storeSensorData(SensorData &sensor_data)
{
    launch_data = SD.open("datalog.csv", FILE_WRITE);
    if (launch_data)
    {
        launch_data.print(sensor_data.time);
        launch_data.print(",");
        launch_data.print(sensor_data.temperature_avbay);
        launch_data.print(",");
        launch_data.print(sensor_data.temperature_engbay);
        launch_data.print(",");
        launch_data.print(sensor_data.altimeter_temp);
        launch_data.print(",");
        launch_data.print(sensor_data.pressure);
        launch_data.print(",");
        launch_data.print(sensor_data.altitude);
        launch_data.print(",");
        launch_data.print(sensor_data.accel_x);
        launch_data.print(",");
        launch_data.print(sensor_data.accel_y);
        launch_data.print(",");
        launch_data.print(sensor_data.accel_z);
        launch_data.print(",");
        launch_data.print(sensor_data.mag_x);
        launch_data.print(",");
        launch_data.print(sensor_data.mag_y);
        launch_data.print(",");
        launch_data.print(sensor_data.mag_z);
        launch_data.print(",");
        launch_data.print(sensor_data.gyro_x);
        launch_data.print(",");
        launch_data.print(sensor_data.gyro_y);
        launch_data.print(",");
        launch_data.println(sensor_data.gyro_z);
        launch_data.close();
    }
    else
    {
        Serial.println("error opening datalog.csv");
    }
}
