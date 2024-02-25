/**************************************************************
 *
 *                     carm_computer_launch1.ino
 *
 *     Author(s):  Daniel Opara, Kenny Chua
 *     Date:       1/6/2024
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
#include "utils.h"
#include "def.h"
#include "bb_setup.h"
#include "MovingAvg.h"

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
MovingAvg moving_accel = MovingAvg();
MovingAvg moving_altitude = MovingAvg();

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
    float gps_speed;
    float gps_angle;
    float gps_altitude;
    int gps_fix;
    int gps_quality;
    int gps_num_satellites;
    int gps_antenna_status;
    int curr_state;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Defining Rocket States and relevant indicators
// - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Define state type and variable which defines mutually exclusive
// flight states to inform what we're supposed

// we'll use this in a later launch
// typedef enum state = {POWER_ON = 0, LAUNCH_READY, POWERED_FLIGHT_PHASE,
//                       COAST_PHASE, APOGEE_PHASE, DROGUE_DEPLOYED,
//                       MAIN_DEPLOY_ATTEMPT, MAIN_DEPLOYED, RECOVERY} state;

typedef enum state
{
    POWER_ON = 0,
    LAUNCHING,
    APOGEE_PHASE,
    DROGUE_DEPLOYED,
    MAIN_DEPLOY_ATTEMPT,
    MAIN_DEPLOYED,
    RECOVERY
} state;

// NOT TRANSMITTING MUCH FOR THESE GUYS
// RECIEVING POWER
// SENSOR VALUES HAVE STABILIZED
bool LAUNCH_READY = false;
// WE ARE ACTUALLY TRANSMITTING HERE
// INDICATORS FROM SENSOR TELL US WE ARE IN THIS MODE
// TRANSITION FROM BURNOUT TO COAST PHASE IS WHEN JERK == 0 AND ACCEL IS NEGATIVE
bool POWERED_FLIGHT_PHASE = false; // have a timer condition in addition to the accelertaio condition

float curr_accel;
float curr_altitude;
float max_altitude;

unsigned int launch_start_time;
state rocket_state;
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Function contracts
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
void readSensors(SensorData &sensor_data, Adafruit_LSM9DS1 &lsm_obj,
                 Adafruit_BMP3XX &bmp_obj, Adafruit_MCP9808 &tempsensor_obj1,
                 Adafruit_MCP9808 &tempsensor_obj2, Adafruit_GPS &gps_obj);
void storeSensorData(SensorData &sensor_data);
void readGPS(SensorData &sensor_data)
{
    // gps reading
    GPS.read();
    if (GPS.newNMEAreceived())
    {
        if (!GPS.parse(GPS.lastNMEA()))
        { // this also sets the newNMEAreceived() flag to false
            return;
        }
    }
    if (GPS.fix)
    {
        sensor_data.gps_fix = GPS.fix;
        sensor_data.gps_quality = GPS.fixquality;
        sensor_data.gps_num_satellites = (int)GPS.satellites;
        sensor_data.gps_antenna_status = (int)GPS.antenna;
        sensor_data.gps_lat = convertToDecimalDegrees(GPS.latitude, GPS.lat);
        sensor_data.gps_long = convertToDecimalDegrees(GPS.longitude, GPS.lon);
        sensor_data.gps_speed = GPS.speed;
        sensor_data.gps_angle = GPS.angle;
        sensor_data.gps_altitude = GPS.altitude;
    }
    else
    {
        sensor_data.gps_fix = GPS.fix;
        sensor_data.gps_quality = 1;
        sensor_data.gps_num_satellites = 1;
        sensor_data.gps_antenna_status = 1;
        sensor_data.gps_lat = 1;
        sensor_data.gps_long = 1;
        sensor_data.gps_speed = 1;
        sensor_data.gps_angle = 1;
        sensor_data.gps_altitude = 1;
    }
}

// TODO: Complete this function. Also test if constantly
//         switching devices is a thing that can be done
void switchSPIDevice(int cs_pin);
void stateDeterminer();

void setup()
{
    rocket_state = POWER_ON;
    // pinMode(J1, OUTPUT);
    // pinMode(J2, OUTPUT);
    bool imu_setup = setupSensorIMU(lsm);
    bool bmp_setup = setupSensorBMP(bmp);
    bool temp_setup1 = setupSensorTemp(tempsensor_avbay, 0x18);
    bool temp_setup2 = setupSensorTemp(tempsensor_engbay, 0x19);
    GPS.begin(9600);
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
    GPS.sendCommand(PGCMD_ANTENNA);

    // Creating an array of measurements to pass to the MovingAvg objects
    // we will touch these sensor just this once
    // outside of readSensors that is
    float altitude_readings[QUEUE_MAX_LENGTH];
    float accleration_readings[QUEUE_MAX_LENGTH];

    for (int i = 0; i < QUEUE_MAX_LENGTH; i++)
    {
        // read sensors
        lsm.read();
        sensors_event_t a, m, g, temp;
        lsm.getEvent(&a, &m, &g, &temp);
        bmp.performReading();

        accleration_readings[i] = calculateAverageAcceleration(a.acceleration.x, a.acceleration.y, a.acceleration.z);
        altitude_readings[i] = bmp.readAltitude(SEALEVELPRESSURE_HPA);
    }

    moving_accel.fill_queue(accleration_readings, 5);
    moving_altitude.fill_queue(altitude_readings, 5);

    // Switch the spi device before setting up the SD card module
    switchSPIDevice(SD_CS);
    bool sd_setup = setupSD();
    // TODO: Add a case where the SD card module setup func returns false
    File init_data = SD.open("init.txt", FILE_WRITE);
    if (init_data)
    {
        (imu_setup) ? (init_data.println("IMU successfully set up")) : (init_data.println("IMU unsuccessfully set up"));
        (bmp_setup) ? (init_data.println("BMP successfully set up")) : (init_data.println("BMP unsuccessfully set up"));
        (temp_setup1) ? (init_data.println("External temp. successfully set up")) : (init_data.println("External temp. unsuccessfully set up"));
        (temp_setup2) ? (init_data.println("Avionics bay temp. successfully set up")) : (init_data.println("Avionics bay temp. unsuccessfully set up"));
        init_data.println("----------------------------------------------------------------------------");
        init_data.close();
    }
    launch_data = SD.open("datalog.csv", FILE_WRITE);
    if (launch_data)
    {
        launch_data.print("STATE");
        launch_data.print(",");
        launch_data.print("time (ms)");
        launch_data.print(",");
        launch_data.print("external temperature (C)"); // in Celcius
        launch_data.print(",");
        launch_data.print("av bay temperature (C)"); // in Celcius
        launch_data.print(",");
        launch_data.print("temp from altimeter (C)"); // in Celcius
        launch_data.print(",");
        launch_data.print("air pressure (kPa)"); // in kiloPascals
        launch_data.print(",");
        launch_data.print("altitude (m)"); // in meters
        launch_data.print(",");
        launch_data.print("partial moving avg altitude (m)"); // in meters
        launch_data.print(",");
        launch_data.print("moving avg altitude (m)"); // in meters
        launch_data.print(",");
        launch_data.print("partial moving avg accel (m/s^2)"); // in meters
        launch_data.print(",");
        launch_data.print("moving avg acceleration (m/s^2)"); // in meters per sec^2
        launch_data.print(",");
        launch_data.print("x acceleration (m/s^2)"); // in meters per sec^2
        launch_data.print(",");
        launch_data.print("y acceleration (m/s^2)"); // in meters per sec^2
        launch_data.print(",");
        launch_data.print("z acceleration (m/s^2)"); // in meters per sec^2
        launch_data.print(",");
        launch_data.print("x magnetic force (gauss)"); // in gauss
        launch_data.print(",");
        launch_data.print("y magnetic force (gauss)"); // in gauss
        launch_data.print(",");
        launch_data.print("z magnetic force (gauss)"); // in gauss
        launch_data.print(",");
        launch_data.print("x gyro (dps)"); // in degrees per sec
        launch_data.print(",");
        launch_data.print("y gyro (dps)"); // in degrees per sec
        launch_data.print(",");
        launch_data.print("z gyro (dps)"); // in degrees per sec
        launch_data.print(",");
        launch_data.print("gps lat");
        launch_data.print(",");
        launch_data.print("gps long");
        launch_data.print(",");
        launch_data.print("gps speed");
        launch_data.print(",");
        launch_data.print("gps angle");
        launch_data.print(",");
        launch_data.print("gps altitude");
        launch_data.print(",");
        launch_data.print("gps fix");
        launch_data.print(",");
        launch_data.print("gps fix quality");
        launch_data.print(",");
        launch_data.print("gps satellites");
        launch_data.print(",");
        launch_data.println("gps antenna");
        launch_data.close();
    }
}

void loop()
{
    SensorData data = {0};
    stateDeterminer();
    readSensors(data, lsm, bmp, tempsensor_avbay, tempsensor_engbay, GPS);
    storeSensorData(data);
}

/*
 * readSensors
 * Parameters: Pointers to an instantiated SensorData struct, IMU sensor object,
 *              barometric pressure sensor object, and two temp sensor objects respectively
 * Purpose: Gets readings from sensors and put them into the aforementioned SensorData struct
 * Returns: Nothing
 * Notes: Pushes the current measurements of the  acceleration and altitude to their
 *          respective queue for moving average calculations. Also handles
 */
void readSensors(SensorData &sensor_data, Adafruit_LSM9DS1 &lsm_obj,
                 Adafruit_BMP3XX &bmp_obj, Adafruit_MCP9808 &tempsensor_obj1,
                 Adafruit_MCP9808 &tempsensor_obj2, Adafruit_GPS &gps_obj)
{
    // imu reading
    lsm_obj.read();
    sensors_event_t a, m, g, temp;
    lsm_obj.getEvent(&a, &m, &g, &temp);

    // bmp reading
    if (!bmp_obj.performReading())
    {
        // just fill the struct with negative values here so it is clear that reading failed
        sensor_data.pressure = -1111111;
        sensor_data.altitude = -1111111;
    }
    else
    {
        sensor_data.pressure = bmp_obj.pressure / 100.0;
        sensor_data.altitude = bmp_obj.readAltitude(SEALEVELPRESSURE_HPA);
    }

    readGPS(sensor_data);

    // store the sensor readings in the struct
    if (rocket_state == POWER_ON)
    {
        sensor_data.time = 0;
    }
    else
    {
        sensor_data.time = millis() - launch_start_time;
    }
    sensor_data.temperature_avbay = tempsensor_obj1.readTempC();
    sensor_data.temperature_engbay = tempsensor_obj2.readTempC();
    sensor_data.accel_x = a.acceleration.x;
    sensor_data.accel_y = a.acceleration.y;
    sensor_data.accel_z = a.acceleration.z;
    sensor_data.mag_x = m.magnetic.x;
    sensor_data.mag_y = m.magnetic.y;
    sensor_data.mag_z = m.magnetic.z;
    sensor_data.gyro_x = g.gyro.x;
    sensor_data.gyro_y = g.gyro.y;
    sensor_data.gyro_z = g.gyro.z;
    sensor_data.curr_state = rocket_state;

    // adding current measurments to our queue
    moving_accel.add_new_measurement((calculateAverageAcceleration(a.acceleration.x, a.acceleration.y, a.acceleration.z)));
    moving_altitude.add_new_measurement(sensor_data.altitude);

    // setting our variables to help track state
    curr_altitude = moving_altitude.get_partial_average();
    curr_accel = moving_accel.get_partial_average();
    if (curr_altitude > max_altitude)
    {
        max_altitude = curr_altitude;
    }
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
        launch_data.print(sensor_data.curr_state);
        launch_data.print(",");
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
        launch_data.print(curr_altitude);
        launch_data.print(",");
        launch_data.print(moving_altitude.get_average());
        launch_data.print(",");
        launch_data.print(curr_accel);
        launch_data.print(",");
        launch_data.print(moving_accel.get_average());
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
        launch_data.print(sensor_data.gyro_z);
        launch_data.print(",");
        launch_data.print(sensor_data.gps_lat);
        launch_data.print(",");
        launch_data.print(sensor_data.gps_long);
        launch_data.print(",");
        launch_data.print(sensor_data.gps_speed);
        launch_data.print(",");
        launch_data.print(sensor_data.gps_angle);
        launch_data.print(",");
        launch_data.print(sensor_data.gps_altitude);
        launch_data.print(",");
        launch_data.print(sensor_data.gps_fix);
        launch_data.print(",");
        launch_data.print(sensor_data.gps_quality);
        launch_data.print(",");
        launch_data.print(sensor_data.gps_num_satellites);
        launch_data.print(",");
        launch_data.println(sensor_data.gps_antenna_status);
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

/*
 * stateDeterminer
 * Parameters: None
 * Purpose: Switches the state the rocket is in based on
 * Returns: Nothing
 * Notes: None
 */
void stateDeterminer()
{
    if (rocket_state == POWER_ON)
    {
        if ((curr_accel > moving_accel.get_average()) && (curr_altitude > moving_altitude.get_average()))
        {
            launch_start_time = millis();
            rocket_state = LAUNCHING;
        }
        return;
    }

    if (rocket_state == LAUNCHING)
    {
        if (curr_altitude < max_altitude)
        {
            rocket_state = APOGEE_PHASE;
        }
        return;
    }

    if (rocket_state == APOGEE_PHASE)
    {
        if (curr_altitude < 700.0)
        {
            rocket_state = MAIN_DEPLOY_ATTEMPT;
        }
        if (!(moving_accel.get_partial_average() > 5.0))
        {
            rocket_state = DROGUE_DEPLOYED;
        }
        return;
    }

    if (rocket_state == MAIN_DEPLOY_ATTEMPT)
    {
        if (moving_accel.get_partial_average() > 1)
        {
            rocket_state = MAIN_DEPLOYED;
        }
        return;
    }

    if (rocket_state == MAIN_DEPLOYED)
    {
        if (curr_altitude < 30.0)
        {
            rocket_state = RECOVERY;
        }
        return;
    }
}