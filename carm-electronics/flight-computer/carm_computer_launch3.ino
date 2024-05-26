#include <Wire.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS1.h> // IMU module
#include "Adafruit_BMP3XX.h"  // BMP module
#include "Adafruit_MCP9808.h" // Temp sensor module
#include <Adafruit_GPS.h>     // GPS module
#include <RH_RF95.h>
#include "def.h"
#include "utils.h"
#include "BBManager.h"
#include "BBsetup.h"
#include "DLTransforms.h"
#include "compression.h"

Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();               // imu
Adafruit_BMP3XX bmp;                                     // barometric pressure sensor
Adafruit_MCP9808 tempsensor_avbay = Adafruit_MCP9808();  // avionics bay thermocouple
Adafruit_MCP9808 tempsensor_engbay = Adafruit_MCP9808(); // engine bay thermocouple
Adafruit_GPS GPS(&GPSSerial);                            // hardware GPS object
File launch_data;                                        // file object for writing data
File error_data;                                         // file object for errors
BBManager bboard_manager = BBManager();
StateDeterminer state_determiner = StateDeterminer();
RH_RF95 rf95(RFM95_CS, RFM95_INT);

const char *callsign = "KC1SIL";
const char *destination_callsign = "APRS"; // generic destination SSID
const int control_field = 0x03;
const int protocol_id = 0xF0;

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

void createAPRSPacket(char *buffer, float lat_degree_minutes, char lat_dir,
                      float lon_degree_minutes, char lon_dir,
                      float speed, float course, int altitude)
{

    // make APRS information field using degree-minute format
    char info_field[100];
    snprintf(info_field, sizeof(info_field), "!%10.4f%c/%11.4f%c>%06.2f/%06.02f /A=%06d",
             lat_degree_minutes, lat_dir,
             lon_degree_minutes, lon_dir,
             course, speed, altitude);

    // formatting the control field and protocol id
    char cf_pi[2];
    sprintf(cf_pi, "%x%x", control_field, protocol_id);

    unsigned short fcs = fcs_calc((unsigned char *)info_field, 100);
    char fcs_str[2];
    snprintf(fcs_str, sizeof(fcs_str), "%02x", fcs);

    // encode AX.25 frame
    snprintf(buffer, 255, "~%s>%s,%s:%s %s~",
             callsign, destination_callsign, cf_pi, info_field, fcs_str);
}

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
    uint16_t init_fails = 0;
    if (init_info)
    {
        if (imu_setup)
        {
            init_info.println("IMU successfully set up");
        }
        else
        {
            init_info.println("IMU unsuccessfully set up");
            init_fails = flip_bit(init_fails, 2, 1);
        }

        if (bmp_setup)
        {
            init_info.println("BMP successfully set up");
        }
        else
        {
            init_info.println("BMP unsuccessfully set up");
            init_fails = flip_bit(init_fails, 3, 1);
        }

        if (temp_setup1)
        {
            init_info.println("External temp. successfully set up");
        }
        else
        {
            init_info.println("External temp. unsuccessfully set up");
            init_fails = flip_bit(init_fails, 4, 1);
        }

        if (temp_setup2)
        {
            init_info.println("Avionics bay temp. successfully set up");
        }
        else
        {
            init_info.println("Avionics bay temp. unsuccessfully set up");
            init_fails = flip_bit(init_fails, 5, 1);
        }
        init_info.println("----------------------------------------------------------------------------");
        init_info.close();
        bboard_manager.failure_flags = init_fails;
        tone(BUZZER_PIN, 4000);
        delay(1000);
        noTone(BUZZER_PIN);
    }

    // Setting up the transeiver
    switchSPIDevice(RFM95_CS);
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, HIGH);
    Serial.println("Feather LoRa TX Test!");
    if (!rf95.init())
    {
        Serial.println("LoRa radio init failed");
        Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    }
    else
    {
        Serial.println("LoRa radio init OK!");
        if (!rf95.setFrequency(RF95_FREQ_2))
        {
            Serial.println("setFrequency failed");
        }
        else
        {
            Serial.print("Set Freq to: ");
            Serial.println(RF95_FREQ_2);
            rf95.setTxPower(23, false);
            for (int i = 0, i < 5; i++)
            {
                char init_success_packet[] = "Hello ground station!";
                rf95.send((uint8_t *)init_success_packet, sizeof(init_success_packet) + 1);
                rf95.waitPacketSent();
            }
        }
    }

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

    switchSPIDevice(RFM95_CS);
    unsigned int *launchmode_d = transform_launchmode(bboard_manager);
    uint64_t[5] launchmode_words = {pack_noschema(launchmode_d)};
    rf95.send((uint8_t *)launchmode_words, sizeof(launchmode_words));
    rf95.waitPacketSent();

    // following APRS AX.25 protocol to transmit to MCC
    char ax25_buffer[255];
    createAPRSPacket(ax25_buffer, static_cast<float>(GPS.latitude), GPS.lat,
                     static_cast<float>(GPS.longitude), GPS.lon,
                     static_cast<float>(GPS.speed),
                     static_cast<float>(GPS.angle), (int)GPS.altitude);
    rf95.send((uint8_t *)ax25_buffer, sizeof(ax25_buffer) + 1);
    rf95.waitPacketSent();
}
