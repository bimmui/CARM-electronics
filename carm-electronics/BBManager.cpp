/**************************************************************
 *
 *                     BBManager.cpp
 *
 *     Author(s):  Daniel Opara
 *     Date:       3/20/2024
 *
 *     Overview: Implementation of BBManager.h
 *
 *
 **************************************************************/

#include "BBManager.h"
#include "def.h"
#include "utils.h"

static const unsigned MAX_ATTEMPTS = 20;
static const unsigned DECIMAL_COUNT = 4;
static const unsigned GPS_DECIMAL_COUNT = 6;

/*
 * setupSensorIMU
 * Parameters: Reference (pointer) to the instantiated IMU sensor object
 * Purpose: Sets up the range at which the sensor's individual components measure
 *           higher ranges = less precision but can measure larger movements
 * Returns: Bool representing whether or not sensor was set up
 * Notes: The sensor was set up in I2C. For more info, read the device documentation:
 *            https://learn.adafruit.com/adafruit-lsm9ds1-accelerometer-plus-gyro-plus-magnetometer-9-dof-breakout/overview
 */

/*
 * BBManager constructor
 * Parameters: None
 * Purpose: Sets up all the sensors with their proper configurations
 * Notes: Writes to disk which sensors were successfully set up and which ones weren't
 *
 */
BBManager::BBManager()
{
    // set up the class vars
    curr_state = state::POWER_ON;
    failure_flags = 0;
    curr_launch_time = 0;
    temperature_engbay = 0;
    external_temp = 0;
    temperature_avbay = 0;
}

void BBManager::initDatalog(File &file_stream)
{
    file_stream = SD.open("datalog.csv", FILE_WRITE);
    if (file_stream)
    {
        file_stream.print("STATE");
        file_stream.print(",");
        file_stream.print("time (ms)");
        file_stream.print(",");
        file_stream.print("external temperature (C)"); // in Celcius
        file_stream.print(",");
        file_stream.print("engine bay temperature (C)"); // in Celcius
        file_stream.print(",");
        file_stream.print("av bay temperature (C)"); // in Celcius
        file_stream.print(",");
        file_stream.print("barometer temp (C)"); // in Celcius
        file_stream.print(",");
        file_stream.print("air pressure (kPa)"); // in kiloPascals
        file_stream.print(",");
        file_stream.print("altitude (m)"); // in meters
        file_stream.print(",");
        file_stream.print("raw altitude (m)"); // in meters
        file_stream.print(",");
        file_stream.print("kf vertical velocity (m/s)"); // in m/s
        file_stream.print(",");
        file_stream.print("kf vertical acceleration (m/s^2)"); // in m/s^2
        file_stream.print(",");
        file_stream.print("kf altitude (m)"); // in m
        file_stream.print(",");
        file_stream.print("x acceleration (m/s^2)"); // in m/s^2
        file_stream.print(",");
        file_stream.print("y acceleration (m/s^2)"); // in m/s^2
        file_stream.print(",");
        file_stream.print("z acceleration (m/s^2)"); // in m/s^2
        file_stream.print(",");
        file_stream.print("x magnetic force (gauss)"); // in gauss
        file_stream.print(",");
        file_stream.print("y magnetic force (gauss)"); // in gauss
        file_stream.print(",");
        file_stream.print("z magnetic force (gauss)"); // in gauss
        file_stream.print(",");
        file_stream.print("x gyro (dps)"); // in degrees per sec
        file_stream.print(",");
        file_stream.print("y gyro (dps)"); // in degrees per sec
        file_stream.print(",");
        file_stream.print("z gyro (dps)"); // in degrees per sec
        file_stream.print(",");
        file_stream.print("gps lat");
        file_stream.print(",");
        file_stream.print("gps long");
        file_stream.print(",");
        file_stream.print("gps speed");
        file_stream.print(",");
        file_stream.print("gps angle");
        file_stream.print(",");
        file_stream.print("gps altitude");
        file_stream.print(",");
        file_stream.print("gps fix");
        file_stream.print(",");
        file_stream.print("gps fix quality");
        file_stream.print(",");
        file_stream.print("gps satellites");
        file_stream.print(",");
        file_stream.print("gps antenna status");
        file_stream.print(",");
        file_stream.println("error flags");
        file_stream.close();
    }
}

/*
 * BBManager destructor
 * Parameters: None
 * Returns: Nothing
 * Purpose: Cleans up mem of BBManager class variables
 * Notes: None
 */
BBManager::~BBManager()
{
    // some instructions
}

void BBManager::setSensors(Adafruit_LSM9DS1 &lsm_obj, Adafruit_BMP3XX &bmp_obj,
                           Adafruit_MCP9808 &tempsensor_obj1, Adafruit_MCP9808 &tempsensor_obj2)
{
    lsm = &lsm_obj;
    bmp = &bmp_obj;
    tempsensor_avbay = &tempsensor_obj1;
    tempsensor_external = &tempsensor_obj2;
    // Serial.println(reinterpret_cast<intptr_t>(*gps));
}

/*
 * readSensorData
 * Parameters: None
 * Purpose: Gets readings from sensors and assigns class vars to their respective sensor output
 * Returns: Nothing
 * Notes: Does not assign value to curr_state, object is handed off to StateDetermination for this
 */
void BBManager::readSensorData()
{
    // imu reading
    lsm->read();
    sensors_event_t a, m, g, temp;
    lsm->getEvent(&a, &m, &g, &temp);

    // use this when we have to care about zeroing the data
    // curr_launch_time = millis() - launch_start_time;
    curr_launch_time = millis();

    // bmp reading
    if (!bmp->performReading())
    {
        pressure = 0;
        altitude = 0;
        barometer_temp = 0;
        failure_flags = flip_bit(failure_flags, 10, 1);
    }
    else
    {
        pressure = bmp->pressure / 100.0;
        altitude = bmp->readAltitude(SEALEVELPRESSURE_HPA) - baro_offset;
        // altitude = (altitude < 0) ? 0 : altitude;
        raw_altitude = bmp->readAltitude(SEALEVELPRESSURE_HPA);
        barometer_temp = bmp->temperature;
        failure_flags = flip_bit(failure_flags, 10, 0);
    }

    temperature_avbay = tempsensor_avbay->readTempC();
    external_temp = tempsensor_external->readTempC();
    accel_x = a.acceleration.x;
    accel_y = a.acceleration.y;
    accel_z = a.acceleration.z;
    mag_x = m.magnetic.x;
    mag_y = m.magnetic.y;
    mag_z = m.magnetic.z;
    gyro_x = g.gyro.x;
    gyro_y = g.gyro.y;
    gyro_z = g.gyro.z;
}

/*
 * writeSensorData
 * Parameters: None
 * Purpose: Prints all sensor data to the launch data file in csv format
 * Returns: Nothing
 * Notes: None
 */
void BBManager::writeSensorData(File &data_stream, File &error_stream)
{
    data_stream = SD.open("datalog.csv", FILE_WRITE);
    if (data_stream)
    {
        // could use static_cast<std::underlying_type_t<state>> to make it more general purpose but we know it's an int
        data_stream.print(static_cast<int>(curr_state));
        data_stream.print(",");
        data_stream.print(curr_launch_time);
        data_stream.print(",");
        data_stream.print(external_temp, DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(temperature_engbay, DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(temperature_avbay, DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(barometer_temp, DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(pressure, DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(altitude, DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(raw_altitude, DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(k_vert_velocity, DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(k_vert_acceleration, DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(k_altitude, DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(accel_x, DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(accel_y, DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(accel_z, DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(mag_x, DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(mag_y, DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(mag_z, DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(gyro_x, DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(gyro_y, DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(gyro_z, DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(gps_lat, GPS_DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(gps_long, GPS_DECIMAL_COUNT);
        data_stream.print(",");
        data_stream.print(gps_speed, DECIMAL_COUNT);
        data_stream.print(",");
        // i dont bother with gps angle and alt bc its straight up casted to a double
        // when we get it from the gps module
        data_stream.print(gps_angle);
        data_stream.print(",");
        data_stream.print(gps_altitude);
        data_stream.print(",");
        data_stream.print(gps_fix);
        data_stream.print(",");
        data_stream.print(gps_quality);
        data_stream.print(",");
        data_stream.print(gps_num_satellites);
        data_stream.print(",");
        data_stream.print(gps_antenna_status);
        data_stream.print(",");
        data_stream.println(failure_flags);
        data_stream.close();
        failure_flags = flip_bit(failure_flags, 5, 0);
    }
    else
    {
        failure_flags = flip_bit(failure_flags, 5, 1);
    }
}

void BBManager::setBaroOffset()
{
    float altitude_readings_sum;

    Serial.println("Calculating barometer offset...");

    // Typically some weird spike in values read when you start things
    // so ignore those first values
    for (int i = 0; i < 100; i++)
    {
        bmp->readAltitude(SEALEVELPRESSURE_HPA);
    }
    for (int i = 0; i < 500; i++)
    {
        altitude_readings_sum += bmp->readAltitude(SEALEVELPRESSURE_HPA);
        delay(50);
    }
    Serial.print("Readings sum: ");
    Serial.println(altitude_readings_sum, DECIMAL_COUNT);

    baro_offset = (altitude_readings_sum / 500.0);

    Serial.print("Barometer offset: ");
    Serial.println(baro_offset, DECIMAL_COUNT);
    delay(3000);

    Serial.println("Complete!");
}
