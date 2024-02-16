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
#include <cppQueue.h>
#include <Adafruit_LSM9DS1.h> // IMU module
#include "Adafruit_BMP3XX.h"  // BMP module
#include "Adafruit_MCP9808.h" // Temp sensor module
#include <Adafruit_GPS.h>     // GPS module
#include "bb_setup.h"
#include "def.h"
#include "utility.h"

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
//   Defining Rocket States and relevant indicators
// - - - - - - - - - - - - - - - - - - - - - - - - - - -

// NOT TRANSMITTING MUCH FOR THESE GUYS
// RECIEVING POWER
bool POWER_ON = true;
// SENSOR VALUES HAVE STABILIZED
bool LAUNCH_READY = false;

// WE ARE ACTUALLY TRANSMITTING HERE
// INDICATORS FROM SENSOR TELL US WE ARE IN THIS MODE
bool LAUNCH_MODE = false;
// TRANSITION FROM BURNOUT TO COAST PHASE IS WHEN JERK == 0 AND ACCEL IS NEGATIVE
bool POWERED_FLIGHT_PHASE = false;
bool COAST_PHASE = false;

bool APOGEE_PHASE = false;
bool DROGUE_DEPLOYED_PHASE = false;
bool MAIN_DEPLOY_ATTEMPT = false;
bool MAIN_DEPLOYED_PHASE = false;
bool RECOVERY_PHASE = false;

// we use a moving average implemented with queues to detect when to change states
cppQueue velocity_q(sizeof(float), QUEUE_MAX_LENGTH, IMPLEMENTATION);
cppQueue accel_q(sizeof(float), QUEUE_MAX_LENGTH, IMPLEMENTATION);
cppQueue altitude_q(sizeof(float), QUEUE_MAX_LENGTH, IMPLEMENTATION);

// a moving sum is also calculated to reduce time spent interating through
//      each index in the array to get the average
float curr_velocity_sum;
float curr_accel_sum;
float curr_altitude_sum;

unsigned int launch_start_time;

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Function contracts
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
void readSensors(SensorData &sensor_data, Adafruit_LSM9DS1 &lsm_obj,
                 Adafruit_BMP3XX &bmp_obj, Adafruit_MCP9808 &tempsensor_obj1,
                 Adafruit_MCP9808 &tempsensor_obj2);
void storeSensorData(SensorData &sensor_data);
// TODO: Complete this function. Also test if constantly
//         switching devices is a thing that can be done
void switchSPIDevice(int cs_pin);
void stateDeterminer();

void setup()
{
    Serial.begin(115200); // serial used for testing
    bool imu_setup = setupSensorIMU(lsm);
    bool bmp_setup = setupSensorBMP(bmp);
    bool temp_setup1 = setupSensorTemp(tempsensor_avbay, 0x18);
    bool temp_setup2 = setupSensorTemp(tempsensor_engbay, 0x19);
    bool gps_setup = setupGPS(GPS);

    // Switch the spi device before setting up the SD card module
    switchSPIDevice(SD_CS);
    bool sd_setup = setupSD(CHIPSELECT);
    // TODO: Add a case where the SD card module setup func returns false
    File init_data = SD.open("init.txt", FILE_WRITE);
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
    stateDeterminer();
    readSensors(data, lsm, bmp, tempsensor_avbay, tempsensor_engbay);
    storeSensorData(data);
}

/*
 * readSensors
 * Parameters: Pointers to an instantiated SensorData struct, IMU sensor object,
 *              barometric pressure sensor object, and two temp sensor objects respectively
 * Purpose: Gets readings from sensors and put them into the aforementioned SensorData struct
 * Returns: Nothing
 * Notes: Pushes the current measurements of the velocity, acceleration, and altitude to their
 *          respective queue
 */
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
    // TODO: Be able to have T+ launch time instead of total time
    //          the Feather was running
    sensor_data.time = millis() - launch_start_time;              // TODO: change this to be T+ time
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

    velocity_q.push(&sensor_data.altitude);
    accel_q.push(&sensor_data.altitude);
    altitude_q.push(&sensor_data.altitude);
}

/*
 * storeSensorData
 * Parameters: A pointer to an instantiated SensorData struct
 * Purpose: Prints every variable in the struct to the launch
 *          data file in csv format
 * Returns: Nothing
 * Notes: None
 */
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
        error_data = SD.open("errlog.txt", FILE_WRITE);
        if (error_data)
        {
            error_data.print(millis() - launch_start_time);
            error_data.print(": ");
            error_data.println("Error logging flight data");
            error_data.close();
        }
    }
}

/*
 * switchSPIDevice
 * Parameters: An integer that represents the CS pin of an SPI device
 * Purpose: Switches the SPI device to communicate with.
 * Returns: Nothing
 * Notes: The Feather can only "talk" to one device at a time so we much switch
 *          the CS pin that is being listened to before using the respective device
 */
void switchSPIDevice(int cs_pin)
{
    // we want to use radio in this
    if (cs_pin == RFM95_CS)
    {
        digitalWrite(SD_CS, HIGH);
        digitalWrite(RFM95_CS, LOW);
    }
    // we want to use the sd card module in this
    if (cs_pin == SD_CS)
    {
        digitalWrite(RFM95_CS, HIGH);
        digitalWrite(SD_CS, LOW);
    }
}