#include <Wire.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS1.h> // IMU module
#include "Adafruit_BMP3XX.h"  // BMP module
#include "Adafruit_MCP9808.h" // Temp sensor module
#include <Adafruit_GPS.h>     // GPS module
#include "def.h"
#include "utils.h"
#include "BBManager.h"
#include "BBsetup.h"

void switchSPIDevice(int cs_pin);

Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();               // imu
Adafruit_BMP3XX bmp;                                     // barometric pressure sensor
Adafruit_MCP9808 tempsensor_avbay = Adafruit_MCP9808();  // avionics bay thermocouple
Adafruit_MCP9808 tempsensor_engbay = Adafruit_MCP9808(); // engine bay thermocouple
Adafruit_GPS GPS(&GPSSerial);                            // hardware GPS object
File launch_data;                                        // file object for writing data
File error_data;                                         // file object for errors
BBManager bboard_manager = BBManager();
StateDeterminer state_determiner = StateDeterminer();

void setup()
{
    bool imu_setup = setupSensorIMU(lsm);
    bool bmp_setup = setupSensorBMP(bmp);
    bool temp_setup1 = setupSensorTemp(tempsensor_avbay, 0x18);
    bool temp_setup2 = setupSensorTemp(tempsensor_engbay, 0x19);
    bool gps_setup = setupGPS(GPS);

    switchSPIDevice(SD_CS);
    bool sd_setup = setupSD();
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
        }

        if (bmp_setup)
        {
            init_info.println("BMP successfully set up");
        }
        else
        {
            init_info.println("BMP unsuccessfully set up");
        }

        if (temp_setup1)
        {
            init_info.println("External temp. successfully set up");
        }
        else
        {
            init_info.println("External temp. unsuccessfully set up");
        }

        if (temp_setup2)
        {
            init_info.println("Avionics bay temp. successfully set up");
        }
        else
        {
            init_info.println("Avionics bay temp. unsuccessfully set up");
        }
        init_info.println("----------------------------------------------------------------------------");
        init_info.close();
    }

    bboard_manager.setSensors(lsm, bmp, tempsensor_avbay, tempsensor_engbay, GPS);
    bboard_manager.initDatalog(launch_data);
}

void loop()
{
    bboard_manager.readSensorData();
    state_determiner.determineState(bboard_manager);
    bboard_manager.writeSensorData(launch_data, error_data);
}

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
