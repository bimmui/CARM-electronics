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
 * BBManager constructor
 * Parameters: All the objects that correspond to their respective breakout boards
 * Purpose: Sets up all the sensors with their proper configurations and writes to disk
 *              which ones are were successfully set up and which ones weren't
 * Notes: None
 *
 */
BBManager::BBManager()
{
    bool imu_setup = setupSensorIMU(lsm);
    bool bmp_setup = setupSensorBMP(bmp);
    bool temp_setup1 = setupSensorTemp(tempsensor_avbay, 0x18);
    bool temp_setup2 = setupSensorTemp(tempsensor_engbay, 0x19);
    // bool gps_setup = setupGPS(GPS);
    bool sd_setup = setupSD();
    errors_bitmask = 0;

    File init_info = SD.open("init.txt", FILE_WRITE);
    if (init_info)
    {
        if (imu_setup)
        {
            init_info.println("IMU successfully set up");
        }
        else
        {
            init_info.println("IMU unsuccessfully set up");
            errors_bitmask = flip_bit(errors_bitmask, 2, 1);
        }

        if (bmp_setup)
        {
            init_info.println("BMP successfully set up");
        }
        else
        {
            init_info.println("BMP unsuccessfully set up");
            errors_bitmask = flip_bit(errors_bitmask, 3, 1);
        }

        if (temp_setup1)
        {
            init_info.println("External temp. successfully set up");
        }
        else
        {
            init_info.println("External temp. unsuccessfully set up");
            errors_bitmask = flip_bit(errors_bitmask, 4, 1);
        }

        if (temp_setup2)
        {
            init_info.println("Avionics bay temp. successfully set up");
        }
        else
        {
            init_info.println("Avionics bay temp. unsuccessfully set up");
            errors_bitmask = flip_bit(errors_bitmask, 0, 1);
        }
        init_info.println("----------------------------------------------------------------------------");
        init_info.close();
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
        launch_data.print("altimeter temp (C)"); // in Celcius
        launch_data.print(",");
        launch_data.print("air pressure (kPa)"); // in kiloPascals
        launch_data.print(",");
        launch_data.print("altitude (m)"); // in meters
        launch_data.print(",");
        launch_data.print("vertical velocity (m/s)"); // in Celcius
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
        launch_data.println("gps antenna status");
        launch_data.print(",");
        launch_data.println("error flags");
        launch_data.close();
    }
}

/*
 * BBManager destructor
 * Parameters: None
 * Returns: Nothing
 * Purpose: Cleans up mem of BBManager objects
 * Notes: None
 *
 */
BBManager::~BBManager()
{
    // some instructions
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
    lsm.read();
    sensors_event_t a, m, g, temp;
    lsm.getEvent(&a, &m, &g, &temp);

    curr_launch_time = millis() - launch_start_time;

    // bmp reading
    if (!bmp.performReading())
    {
        pressure = 0;
        altitude = 0;
    }
    else
    {
        pressure = bmp.pressure / 100.0;
        altitude = bmp.readAltitude(SEALEVELPRESSURE_HPA);
    }

    temperature_avbay = tempsensor_avbay.readTempC();
    temperature_engbay = tempsensor_engbay.readTempC();
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

    // TODO: Add kalman filtering code that calculates vertical velocity
}

/*
 * writeSensorData
 * Parameters: None
 * Purpose: Prints all sensor data to the launch data file in csv format
 * Returns: Nothing
 * Notes: None
 */
void BBManager::writeSensorData()
{
    launch_data = SD.open("datalog.csv", FILE_WRITE);
    if (launch_data)
    {
        // could use static_cast<std::underlying_type_t<state>> to make it more general purpose but we know it's an int
        launch_data.print(static_cast<int>(curr_state));
        launch_data.print(",");
        if (curr_state == state::POWER_ON)
        {
            launch_data.print(0);
        }
        else
        {
            launch_data.print(curr_launch_time);
        }
        launch_data.print(curr_launch_time);
        launch_data.print(",");
        launch_data.print(temperature_avbay);
        launch_data.print(",");
        launch_data.print(temperature_engbay);
        launch_data.print(",");
        launch_data.print(altimeter_temp);
        launch_data.print(",");
        launch_data.print(pressure);
        launch_data.print(",");
        launch_data.print(altitude);
        launch_data.print(",");
        launch_data.print(accel_x);
        launch_data.print(",");
        launch_data.print(accel_y);
        launch_data.print(",");
        launch_data.print(accel_z);
        launch_data.print(",");
        launch_data.print(mag_x);
        launch_data.print(",");
        launch_data.print(mag_y);
        launch_data.print(",");
        launch_data.print(mag_z);
        launch_data.print(",");
        launch_data.print(gyro_x);
        launch_data.print(",");
        launch_data.print(gyro_y);
        launch_data.print(",");
        launch_data.print(gyro_z);
        launch_data.print(",");
        launch_data.print(gps_lat);
        launch_data.print(",");
        launch_data.print(gps_long);
        launch_data.print(",");
        launch_data.print(gps_speed);
        launch_data.print(",");
        launch_data.print(gps_angle);
        launch_data.print(",");
        launch_data.print(gps_altitude);
        launch_data.print(",");
        launch_data.print(gps_fix);
        launch_data.print(",");
        launch_data.print(gps_quality);
        launch_data.print(",");
        launch_data.print(gps_num_satellites);
        launch_data.print(",");
        launch_data.println(gps_antenna_status);
        launch_data.close();
    }
    else
    {
        error_data = SD.open("errlog.txt", FILE_WRITE);
        if (error_data)
        {
            error_data.print(curr_launch_time);
            error_data.print(": ");
            error_data.println("Error logging flight data");
            error_data.close();
        }
    }
}

/*
 * setupSensorIMU
 * Parameters: Reference (pointer) to the instantiated IMU sensor object
 * Purpose: Sets up the range at which the sensor's individual components measure
 *           higher ranges = less precision but can measure larger movements
 * Returns: Bool representing whether or not sensor was set up
 * Notes: The sensor was set up in I2C. For more info, read the device documentation:
 *            https://learn.adafruit.com/adafruit-lsm9ds1-accelerometer-plus-gyro-plus-magnetometer-9-dof-breakout/overview
 */
bool BBManager::setupSensorIMU(Adafruit_LSM9DS1 &lsm_obj)
{
    Serial.println("Setting up IMU...");
    for (int attempts = 0; attempts < MAX_ATTEMPTS; attempts++)
    {
        if (!lsm_obj.begin())
        {
            Serial.println("Failed to communicate with LSM9DS1.");
        }
        else
        {
            lsm_obj.setupAccel(lsm_obj.LSM9DS1_ACCELRANGE_2G);
            lsm_obj.setupMag(lsm_obj.LSM9DS1_MAGGAIN_4GAUSS);
            lsm_obj.setupGyro(lsm_obj.LSM9DS1_GYROSCALE_245DPS);
            Serial.println("Complete!");
            return true;
        }
        delay(2000);
    }
    return false;
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
bool BBManager::setupSensorBMP(Adafruit_BMP3XX &bmp_obj)
{
    Serial.println("Setting up barometric pressure sensor...");
    for (int attempts = 0; attempts < MAX_ATTEMPTS; attempts++)
    {
        if (!bmp_obj.begin_I2C())
        {
            Serial.println("Could not find a valid BMP3 sensor, check wiring!");
        }
        else
        {
            // Set up oversampling and filter initialization
            bmp_obj.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
            bmp_obj.setPressureOversampling(BMP3_OVERSAMPLING_4X);
            bmp_obj.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
            bmp_obj.setOutputDataRate(BMP3_ODR_50_HZ);
            Serial.println("Complete!");
            return true;
        }
        delay(2000);
    }
    return false;
}

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
bool BBManager::setupSensorTemp(Adafruit_MCP9808 &tempsensor_obj, uint8_t address)
{
    Serial.println("Setting up temperature sensor...");
    for (int attempts = 0; attempts < MAX_ATTEMPTS; attempts++)
    {
        if (!tempsensor_obj.begin(address))
        {
            Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct.");
        }
        else
        {
            tempsensor_obj.setResolution(3);
            tempsensor_obj.wake();
            Serial.println("Complete!");
            return true;
        }
        delay(2000);
    }
    return false;
}

/*
 * setupSD
 * Parameters: The digital pin the SD card module is connected to
 * Purpose: Initailizes the SD card module
 * Returns: Bool representing whether or not the module was set up
 * Notes: Documentation on the module can be found here
 *          https://learn.adafruit.com/adafruit-micro-sd-breakout-board-card-tutorial/introduction
 */
bool BBManager::setupSD()
{
    for (int attempts = 0; attempts < MAX_ATTEMPTS; attempts++)
    {
        if (!SD.begin(SD_CS))
        {
            delay(2000);
        }
        else
        {
            Serial.println("Complete!");
            return true;
        }
        delay(2000);
    }
    return false;
}

/*
 * setupGPS
 * Parameters:
 * Purpose:
 * Returns:
 * Notes:
 * TODO: Need to figure how setting up the GPS module can go wrong so we can return
 *          false when it does
 */
bool BBManager::setupGPS(Adafruit_GPS &gps_obj)
{
    gps_obj.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    gps_obj.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    gps_obj.sendCommand(PGCMD_ANTENNA);
    return true;
}