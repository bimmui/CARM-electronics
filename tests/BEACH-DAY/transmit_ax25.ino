#include <Adafruit_GPS.h>
#include <RH_RF95.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#if defined(ADAFRUIT_FEATHER_M0) || defined(ADAFRUIT_FEATHER_M0_EXPRESS) || defined(ARDUINO_SAMD_FEATHER_M0) // Feather M0 w/Radio
#define RFM95_CS 8
#define RFM95_INT 3
#define RFM95_RST 4

#endif

#define GPSSerial Serial1
#define RF95_FREQ 434.0
#define GPSECHO false

Adafruit_GPS GPS(&GPSSerial);
RH_RF95 rf95(RFM95_CS, RFM95_INT);

const char *callsign = "KC1SIL";
const char *destination_callsign = "APRS"; // generic destination SSID
const int control_field = 0x03;
const int protocol_id = 0xF0;

uint32_t timer = millis();

static const unsigned short ccitt_table[256] = {

    // from http://www.ietf.org/rfc/rfc1549.txt

    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78

};

/*
 * Use this for an AX.25 frame.
 */

unsigned short fcs_calc(unsigned char *data, int len)
{
    unsigned short crc = 0xffff;
    int j;

    for (j = 0; j < len; j++)
    {

        crc = ((crc) >> 8) ^ ccitt_table[((crc) ^ data[j]) & 0xff];
    }

    return (crc ^ 0xffff);
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
    // Serial.println(info_field);

    // formating the control field and protocol id
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
    // GPS setup
    Serial.begin(115200);
    while (!Serial)
        delay(1);
    Serial.println("Adafruit GPS library basic parsing test!");
    GPS.begin(9600);
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
    GPS.sendCommand(PGCMD_ANTENNA);
    delay(1000);
    GPSSerial.println(PMTK_Q_RELEASE);

    // LoRa transeiver setup
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, HIGH);

    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);

    while (!rf95.init())
    {
        Serial.println("LoRa radio init failed");
        Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
        while (1)
            ;
    }
    Serial.println("LoRa radio init OK!");
    if (!rf95.setFrequency(RF95_FREQ))
    {
        Serial.println("setFrequency failed");
        while (1)
            ;
    }
    Serial.print("Set Freq to: ");
    Serial.println(RF95_FREQ);
    rf95.setTxPower(23, false);
}

void loop()
{
    char c = GPS.read();
    if (GPS.newNMEAreceived())
    {
        if (!GPS.parse(GPS.lastNMEA()))
            return;
    }

    if (millis() - timer > 2000)
    {
        if (GPS.fix)
        {
            char ax25_buffer[255];
            createAPRSPacket(ax25_buffer, static_cast<float>(GPS.latitude), GPS.lat, static_cast<float>(GPS.longitude), GPS.lon, static_cast<float>(GPS.speed), static_cast<float>(GPS.angle), (int)GPS.altitude);
            Serial.println(ax25_buffer);
            // Serial.print(GPS.latitude, 4);
            // Serial.print(GPS.lat);
            // Serial.print(", ");
            // Serial.print(GPS.longitude, 4);
            // Serial.println(GPS.lon);
            // Serial.print("Speed (knots): ");
            // Serial.println(GPS.speed);
            // Serial.print("Angle: ");
            // Serial.println(GPS.angle);
            // Serial.print("Altitude: ");
            // Serial.println(GPS.altitude);
            rf95.send((uint8_t *)ax25_buffer, sizeof(ax25_buffer) + 1);
            rf95.waitPacketSent();
        }
    }
}