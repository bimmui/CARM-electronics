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
        file_stream.print("av bay temperature (C)"); // in Celcius
        file_stream.print(",");
        file_stream.print("barometer temp (C)"); // in Celcius
        file_stream.print(",");
        file_stream.print("air pressure (kPa)"); // in kiloPascals
        file_stream.print(",");
        file_stream.print("altitude (m)"); // in meters
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
                           Adafruit_MCP9808 &tempsensor_obj1, Adafruit_MCP9808 &tempsensor_obj2,
                           Adafruit_GPS &gps_obj)
{
    lsm = &lsm_obj;
    bmp = &bmp_obj;
    tempsensor_avbay = &tempsensor_obj1;
    tempsensor_engbay = &tempsensor_obj2;
    gps = &gps_obj;
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

    curr_launch_time = millis() - launch_start_time;

    // bmp reading
    if (!bmp->performReading())
    {
        pressure = 0;
        altitude = 0;
    }
    else
    {
        pressure = bmp->pressure / 100.0;
        altitude = bmp->readAltitude(SEALEVELPRESSURE_HPA);
    }

    temperature_avbay = tempsensor_avbay->readTempC();
    temperature_engbay = tempsensor_engbay->readTempC();
    accel_x = a.acceleration.x;
    accel_y = a.acceleration.y;
    accel_z = a.acceleration.z;
    mag_x = m.magnetic.x;
    mag_y = m.magnetic.y;
    mag_z = m.magnetic.z;
    gyro_x = g.gyro.x;
    gyro_y = g.gyro.y;
    gyro_z = g.gyro.z;

    // TODO: Add code that gets readings from the GPS
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
        data_stream.print(external_temp);
        data_stream.print(",");
        data_stream.print(temperature_engbay);
        data_stream.print(",");
        data_stream.print(barometer_temp);
        data_stream.print(",");
        data_stream.print(pressure);
        data_stream.print(",");
        data_stream.print(altitude);
        data_stream.print(",");
        data_stream.print(k_vert_velocity);
        data_stream.print(",");
        data_stream.print(k_vert_acceleration);
        data_stream.print(",");
        data_stream.print(k_altitude);
        data_stream.print(",");
        data_stream.print(accel_x);
        data_stream.print(",");
        data_stream.print(accel_y);
        data_stream.print(",");
        data_stream.print(accel_z);
        data_stream.print(",");
        data_stream.print(mag_x);
        data_stream.print(",");
        data_stream.print(mag_y);
        data_stream.print(",");
        data_stream.print(mag_z);
        data_stream.print(",");
        data_stream.print(gyro_x);
        data_stream.print(",");
        data_stream.print(gyro_y);
        data_stream.print(",");
        data_stream.print(gyro_z);
        data_stream.print(",");
        data_stream.print(gps_lat);
        data_stream.print(",");
        data_stream.print(gps_long);
        data_stream.print(",");
        data_stream.print(gps_speed);
        data_stream.print(",");
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
    }
    else
    {
        error_stream = SD.open("errlog.txt", FILE_WRITE);
        if (error_stream)
        {
            error_stream.print(curr_launch_time);
            error_stream.print(": ");
            error_stream.println("Error logging flight data");
            error_stream.close();
        }
    }
}

/*
 * setupSensorBMP
 * Parameters: Reference (pointer) to the instantiated barometric pressure sensor object
 * Purpose: Configures various settings of the BMP3 sensor to control the oversampling of temperature and pressure,
 *          set the IIR (Infinite Impulse Response) filter coefficient, and define the output data rate
 * Returns: Bool representing whether or not sensor was set up
 * Notes: The sensor was set up in I2C. For more info, read the device documentation:
 *            https://learn.adafruit.com/adafruit-bmp388-bmp390-bmp3xx/overview
 */
// bool BBManager::setupSensorBMP(Adafruit_BMP3XX &bmp_obj) {
//   Serial.println("Setting up barometric pressure sensor...");
//   for (int attempts = 0; attempts < MAX_ATTEMPTS; attempts++) {
//     if (!bmp_obj.begin_I2C()) {
//       Serial.println("Could not find a valid BMP3 sensor, check wiring!");
//     } else {
//       // Set up oversampling and filter initialization
//       bmp_obj.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
//       bmp_obj.setPressureOversampling(BMP3_OVERSAMPLING_4X);
//       bmp_obj.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
//       bmp_obj.setOutputDataRate(BMP3_ODR_50_HZ);
//       Serial.println("Complete!");
//       return true;
//     }
//     delay(2000);
//   }
//   return false;
// }

/*
 * setupSensorTemp
 * Parameters: Reference (pointer) to the instantiated temp sensor object and address of the temperature sensor
 * Purpose: Connects the sensor to its address and sets the precision of measurements
 * Returns: Bool representing whether or not sensor was set up
 * Notes: The sensor was set up in I2C. The resolution of measurements impacts the time it takes to collect sample data,
 *            recommended to check out test/feather_scripts and view the comments on the example if you want to better understand how it works.
 *            For more info, read the device documentation:
 *            https://learn.adafruit.com/adafruit-mcp9808-precision-i2c-temperature-sensor-guide/overview
 */
// bool BBManager::setupSensorTemp(Adafruit_MCP9808 &tempsensor_obj, uint8_t address) {
//   Serial.println("Setting up temperature sensor...");
//   for (int attempts = 0; attempts < MAX_ATTEMPTS; attempts++) {
//     if (!tempsensor_obj.begin(address)) {
//       Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct.");
//     } else {
//       tempsensor_obj.setResolution(3);
//       tempsensor_obj.wake();
//       Serial.println("Complete!");
//       return true;
//     }
//     delay(2000);
//   }
//   return false;
// }

/*
 * setupSD
 * Parameters: The digital pin the SD card module is connected to
 * Purpose: Initailizes the SD card module
 * Returns: Bool representing whether or not the module was set up
 * Notes: Documentation on the module can be found here
 *          https://learn.adafruit.com/adafruit-micro-sd-breakout-board-card-tutorial/introduction
 */
// bool BBManager::setupSD() {
//   for (int attempts = 0; attempts < MAX_ATTEMPTS; attempts++) {
//     if (!SD.begin(SD_CS)) {
//       delay(2000);
//     } else {
//       Serial.println("Complete!");
//       return true;
//     }
//     delay(2000);
//   }
//   return false;
// }

/*
 * setupGPS
 * Parameters:
 * Purpose:
 * Returns:
 * Notes:
 * TODO: Need to figure how setting up the GPS module can go wrong so we can return
 *          false when it does
 */
