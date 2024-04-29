#include <Wire.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS1.h> // IMU module
#include "Adafruit_BMP3XX.h"  // BMP module
#include "Adafruit_MCP9808.h" // Temp sensor module
#include <Adafruit_GPS.h>     // GPS module

Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();
File dataFile;

bool setup_SD()
{
    for (int attempts = 0; attempts < 20; attempts++)
    {
        if (!SD.begin(13))
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

bool setup_IMU(Adafruit_LSM9DS1 &lsm_obj)
{
    Serial.println("Setting up IMU...");
    for (int attempts = 0; attempts < 20; attempts++)
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

void setup()
{
    Serial.begin(115200);

    digitalWrite(8, HIGH);
    digitalWrite(13, LOW);
    setup_IMU(lsm);

    // Initialize the SD card
    setup_SD();

    File init_info = SD.open("init.txt", FILE_WRITE);
    if (init_info)
    {
        init_info.println("----------------------------------------------------------------------------");
        Serial.println("i did it");
        init_info.close();
    }
    else
    {
        Serial.println("fuyck");
    }

    // Create a new file on the SD card
    dataFile = SD.open("uncal.csv", FILE_WRITE);
    if (dataFile)
    {
        dataFile.println("Accel_X,Accel_Y,Accel_Z,Mag_X,Mag_Y,Mag_Z,Gyro_X,Gyro_Y,Gyro_Z");
        dataFile.close();
    }
    else
    {
        Serial.println("failing for some reason");
    }
}

void loop()
{
    lsm.read();
    sensors_event_t a, m, g, temp;
    lsm.getEvent(&a, &m, &g, &temp);

    // Open the file in append mode
    dataFile = SD.open("uncal.csv", FILE_WRITE);
    if (dataFile)
    {
        // Write sensor data to the file
        dataFile.print(a.acceleration.x);
        dataFile.print(",");
        dataFile.print(a.acceleration.y);
        dataFile.print(",");
        dataFile.print(a.acceleration.z);
        dataFile.print(",");

        dataFile.print(m.magnetic.x);
        dataFile.print(",");
        dataFile.print(m.magnetic.y);
        dataFile.print(",");
        dataFile.print(m.magnetic.z);
        dataFile.print(",");

        dataFile.print(g.gyro.x);
        dataFile.print(",");
        dataFile.print(g.gyro.y);
        dataFile.print(",");
        dataFile.println(g.gyro.z);

        // Close the file
        dataFile.close();
    }
    else
    {
        Serial.println("Error opening file for writing!");
    }
}