/*
  StandardDeviations.ino : Arduino example sketch to compute the standard
  deviations of the sensors used for the altitude estimation.
*/

#include <Wire.h>
#include <math.h>
#include <Adafruit_LSM9DS1.h>
#include "Adafruit_BMP3XX.h" // BMP module

#define SEALEVELPRESSURE_HPA (1014.8)
// BMP_SCK == SCL
#define BMP_SCK 18
// BMP_CS == CS
#define BMP_CS 17

Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();
Adafruit_BMP3XX bmp;

// Arduino setup and loop functions
void setup(void)
{
    // Begin serial comms
    Serial.begin(115200);
    while (!Serial)
        ;

    // Initialize barometer
    if (!bmp.begin_I2C())
    {
        Serial.println("Could not find a valid BMP3 sensor, check wiring!");
        while (1)
            ;
    }
    else
    {
        // Set up oversampling and filter initialization
        bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
        bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
        bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
        bmp.setOutputDataRate(BMP3_ODR_50_HZ);
        Serial.println("BMP init complete!");
    }
    delay(2000);
    if (!lsm.begin())
    {
        Serial.println("Failed to communicate with LSM9DS1.");
    }
    else
    {
        lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_16G);
        lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
        lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_2000DPS);
        Serial.println("IMU init complete!");
    }
    delay(2000);
}

void loop()
{
    sensors_event_t accel, gyro;
    lsm.getEvent(&accel, &gyro, NULL, NULL);

    float accelData[3] = {accel.acceleration.x, accel.acceleration.y, accel.acceleration.z};
    float gyroData[3] = {gyro.gyro.x, gyro.gyro.y, gyro.gyro.z};

    float baroPressure = bmp.readPressure() / 100.0;

    Serial.print("Accel: ");
    Serial.print(accelData[0]);
    Serial.print(", ");
    Serial.print(accelData[1]);
    Serial.print(", ");
    Serial.print(accelData[2]);
    Serial.print(" | Gyro: ");
    Serial.print(gyroData[0]);
    Serial.print(", ");
    Serial.print(gyroData[1]);
    Serial.print(", ");
    Serial.print(gyroData[2]);
    Serial.print(" | Baro: ");
    Serial.println(baroPressure);

    delay(1000);
}