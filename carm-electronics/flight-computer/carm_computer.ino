/**************************************************************
 *
 *                     carm_computer.ino
 *
 *     Author(s):  Daniel Opara, Kenny Chua
 *     Date:       2/4/2024
 *
 *     Overview: Driver code for the rocket computer. Gets readings from the sensors
 *                  connected to it and transmits the data to the ground station.
 *
 *
 **************************************************************/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS1.h> // IMU module
#include "Adafruit_BMP3XX.h"  // BMP module
#include "Adafruit_MCP9808.h" // Temp sensor module
#include "sensor_setup.h"
#include <SD.h> // SD card module

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Defining digital pins, constants, and sensor objects
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// BMP_SCK == SCL
#define BMP_SCK 18
// BMP_CS == CS
#define BMP_CS 17
#define SEALEVELPRESSURE_HPA (1013.25)
const int chipSelect = 22; // SD card module

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Defining Rocket States
// - - - - - - - - - - - - - - - - - - - - - - - - - - -

// NOT TRANSMITTING MUCH FOR THESE GUYS
// RECIEVING POWER
bool POWER_ON
// SENSOR VALUES HAVE STABILIZED
bool LAUNCH_READY

// WE ARE ACTUALLY TRANSMITTING HERE
// INDICATORS FROM SENSOR TELL US WE ARE IN THIS MODE
bool LAUNCH_MODE
// TRANSITION FROM BURNOUT TO COAST PHASE IS WHEN JERK == 0 AND ACCEL IS NEGATIVE
bool POWERED_FLIGHT_PHASE 
bool COAST_PHASE

bool APOGEE_PHASE // determined by barometric altitude
bool DROGUE_DEPLOYMENT 
bool MAIN_DEPLOYMENT
bool RECOVERY_PHASE




Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();
Adafruit_BMP3XX bmp;
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

File SD_File;

struct SensorData
{
    float time;
    float temperature;
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
};


void setup()
{
    Serial.begin(115200);
    setupSensorIMU(lsm);
    setupSensorBMP(bmp);
    setupSensorTemp(tempsensor);
    tempsensor.wake();
    setupSDCard(writeToSD);
}

void loop(writeToSD)
{
    // get readings from all the sensors
    lsm.read();
    sensors_event_t a, m, g, temp;
    if (!bmp.performReading())
    {
        Serial.println("Failed to perform reading :(");
        return;
    }

    // create a new SensorData object
    new SensorData data;
    data.time = millis();



    // print out readings
    Serial.print("Temperature Sensor = ");
    Serial.println(tempsensor.readTempC());
    data.temperature = tempsensor.readTempC();

    Serial.print("Temperature = ");
    Serial.print(bmp.temperature);
    Serial.println(" *C");
    data.altimeter_temp = bmp.temperature;
    

    Serial.print("Pressure = ");
    Serial.print(bmp.pressure / 100.0);
    Serial.println(" kPa");
    data.pressure = bmp.pressure / 100.0;

    Serial.print("Approx. Altitude = ");
    Serial.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");
    data.altitude = bmp.readAltitude(SEALEVELPRESSURE_HPA);

    Serial.println();
    lsm.getEvent(&a, &m, &g, &temp);
    Serial.print("Accel X: ");
    Serial.print(a.acceleration.x);
    data.accel_x = a.acceleration.x;


    Serial.print(" Y: ");
    Serial.print(a.acceleration.y);
    data.accel_y = a.acceleration.y;

    Serial.print(" Z: ");
    Serial.println(a.acceleration.z);
    data.accel_z = a.acceleration.z;

    Serial.print("Mag X: ");
    Serial.print(m.magnetic.x);
    data.mag_x = m.magnetic.x;

    Serial.print(" Y: ");
    Serial.print(m.magnetic.y);
    data.mag_y = m.magnetic.y;

    Serial.print(" Z: ");
    Serial.println(m.magnetic.z);
    data.mag_z = m.magnetic.z;

    Serial.print("Gyro X: ");
    Serial.print(g.gyro.x);
    data.gyro_x = g.gyro.x;

    Serial.print(" Y: ");
    Serial.print(g.gyro.y);
    data.gyro_y = g.gyro.y;

    Serial.print(" Z: ");
    Serial.println(g.gyro.z);
    data.gyro_z = g.gyro.z;


    //write the SensorData object to the SD card in csv format
    for (int i = 0; i < 14; i++)
    {
        writeToSD.print(data[i]);
        writeToSD.print(",");
    }
    writeToSD.println();

    //Delays for 2 seconds
    //TODO maybe change this so that the radio transmission is in 2 second increments but the data writeout is for a much higher frequency



    delay(2000);

    bool timingToSend = false;

    if (timingToSend)
    {
        // Do the bit packing and send the data to the groundstation
    }


}


 
void setupSDCard(writeToSD)
{
  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
   pinMode(chipSelect, OUTPUT);
 
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialization done.");
 
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  writeToSD = SD.open("Data_output.csv", FILE_WRITE);
 
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.csv...");
    myFile.println();
	// close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  return;
}
