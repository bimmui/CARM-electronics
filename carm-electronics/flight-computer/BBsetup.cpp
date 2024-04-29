/**************************************************************
 *
 *                     bb_setup.h
 *
 *     Author(s):  Daniel Opara,
 *     Date:       1/6/2024
 *
 *     Overview: Contains functions for setting up configurations for the breakout
 *                  boards connected to the Feather M0, this includes the sensor breakout
 *                  boards, the SD card module, GPS module, etc.
 *
 *
 **************************************************************/

#include "BBsetup.h"
#include "def.h"

const int MAX_ATTEMPTS = 20;
const uint16_t CALIBRATE_ITERS = 3000;

/*
 * setupSensorIMU
 * Parameters: Reference (pointer) to the instantiated IMU sensor object
 * Purpose: Sets up the range at which the sensor's individual components measure
 *           higher ranges = less precision but can measure larger movements
 * Returns: Bool representing whether or not sensor was set up
 * Notes: The sensor was set up in I2C. For more info, read the device documentation:
 *            https://learn.adafruit.com/adafruit-lsm9ds1-accelerometer-plus-gyro-plus-magnetometer-9-dof-breakout/overview
 */
bool setup_IMU(Adafruit_LSM9DS1 &lsm_obj)
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
            lsm_obj.setupAccel(lsm_obj.LSM9DS1_ACCELRANGE_16G);
            lsm_obj.setupMag(lsm_obj.LSM9DS1_MAGGAIN_4GAUSS);
            lsm_obj.setupGyro(lsm_obj.LSM9DS1_GYROSCALE_2000DPS);
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
bool setup_BMP(Adafruit_BMP3XX &bmp_obj)
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
bool setup_Temp(Adafruit_MCP9808 &tempsensor_obj, uint8_t address)
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
bool setup_SD()
{
    for (int attempts = 0; attempts < MAX_ATTEMPTS; attempts++)
    {
        if (!SD.begin(SD_CS))
        {
            Serial.println("SD card not setting up");
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
bool setup_GPS(Adafruit_GPS &gps_obj)
{
    gps_obj.begin(9600);
    gps_obj.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    gps_obj.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    gps_obj.sendCommand(PGCMD_ANTENNA);
    return true;
}
