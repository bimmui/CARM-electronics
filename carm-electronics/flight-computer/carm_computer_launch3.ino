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

Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();               // imu
Adafruit_BMP3XX bmp;                                     // barometric pressure sensor
Adafruit_MCP9808 tempsensor_avbay = Adafruit_MCP9808();  // avionics bay thermocouple
Adafruit_MCP9808 tempsensor_engbay = Adafruit_MCP9808(); // engine bay thermocouple
Adafruit_GPS GPS(&GPSSerial);                            // hardware GPS object
File launch_data;                                        // file object for writing data
File error_data;                                         // file object for errors
BBManager bboard_manager = BBManager();
StateDeterminer state_determiner = StateDeterminer();
// RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup()
{
    bool imu_setup = setup_IMU(lsm);
    bool bmp_setup = setup_BMP(bmp);
    bool temp_setup1 = setup_tempsens(tempsensor_avbay, 0x18);
    bool temp_setup2 = setup_tempsens(tempsensor_engbay, 0x19);
    GPS.begin(9600);
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    GPS.sendCommand(PGCMD_ANTENNA);
    GPSSerial.println(PMTK_Q_RELEASE);
    delay(1000);

    switchSPIDevice(SD_CS);
    pinMode(BUZZER_PIN, OUTPUT);
    bool sd_setup = setup_SD();
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
        tone(BUZZER_PIN, 4000); // Send 1KHz sound signal...
        delay(1000);            // ...for 1 sec
        noTone(BUZZER_PIN);     // Stop sound...
    }

    // Setting up the transeiver
    // switchSPIDevice(RFM95_CS);
    // pinMode(RFM95_RST, OUTPUT);
    // digitalWrite(RFM95_RST, HIGH);
    // Serial.println("Feather LoRa TX Test!");
    // while (!rf95.init()) {
    //   Serial.println("LoRa radio init failed");
    //   Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    //   while (1)
    //     ;
    // }
    // Serial.println("LoRa radio init OK!");
    // if (!rf95.setFrequency(RF95_FREQ_2)) {
    //   Serial.println("setFrequency failed");
    //   while (1)
    //     ;
    // }
    // Serial.print("Set Freq to: ");
    // Serial.println(RF95_FREQ_2);
    // rf95.setTxPower(23, false);

    // handling the breakout board setup
    switchSPIDevice(SD_CS);
    bboard_manager.setSensors(lsm, bmp, tempsensor_avbay, tempsensor_engbay);
    bboard_manager.initDatalog(launch_data);
    bboard_manager.setBaroOffset();
}

void loop()
{
    switchSPIDevice(SD_CS);
    bboard_manager.readSensorData();

    // TODO: Not majorly important but i couldnt figure out how to properly pass a reference
    //        to the GPS obj but it works in the main func so im not gonna worry too much about that,
    //         not as clean as i'd hoped but functionality is priority
    GPS.read();
    if (GPS.newNMEAreceived())
    {

        // Serial.println("im in");
        if (!GPS.parse(GPS.lastNMEA()))
        {
            // does nothing, ik its stupid but the logic
            // wouldnt work for some reason if I didnt do it this way
        }
    }
    bboard_manager.gps_fix = (int)GPS.fix;
    bboard_manager.gps_quality = (int)GPS.fixquality;
    if (GPS.fix)
    {
        // Serial.println("good fix");
        bboard_manager.gps_lat = convert_to_decimaldegrees(static_cast<float>(GPS.latitude), GPS.lat);
        bboard_manager.gps_long = convert_to_decimaldegrees(static_cast<float>(GPS.longitude), GPS.lon);
        bboard_manager.gps_speed = static_cast<float>(GPS.speed);
        bboard_manager.gps_angle = static_cast<float>(GPS.angle);
        bboard_manager.gps_altitude = static_cast<float>(GPS.altitude);
        bboard_manager.gps_num_satellites = (int)GPS.satellites;
        bboard_manager.gps_antenna_status = (int)GPS.antenna;
    }

    state_determiner.determineState(bboard_manager);
    bboard_manager.writeSensorData(launch_data, error_data);

    // switchSPIDevice(RFM95_CS);
    // char radiopacket[] = "Hello ground station";
    // rf95.send((uint8_t *)radiopacket, strlen(radiopacket) + 1);
    // rf95.waitPacketSent();
}
