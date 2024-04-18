/**************************************************************
 *
 *                     carm_computer_launch1.ino
 *
 *     Author(s):  Daniel Opara
 *     Date:       1/6/2024
 *
 *     Overview: Driver code for the rocket computer. Gets readings from the sensors
 *                  connected to it and writes it to an SD card.
 *
 *
 **************************************************************/

#include <RH_RF95.h>
#include "StateDetermination.h"
#include "DLTransforms.h"
#include "untransformed.h"
#include "decompression.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Initializing revelant objects and structs
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Singleton instance of the radio driver
RH_RF95 rf96(RFM96_CS, RFM96_INT);

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Function contracts
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
uint16_to_binary_string(uint16_t num, char *result);

void setup()
{
}

void loop()
{
    uint64_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    Serial.println("Waiting for reply...");
    if (rf96.waitAvailableTimeout(1000))
    {
        // Should be a reply message for us now
        if (rf96.recv(buf, &len))
        {
            // the first word will always have the state in the first four bits
            state curr_state = get_state_from_bits(buf[0]);
            switch (curr_state)
            {
            case state::POWER_ON:
            {
                unsigned int *poweron_d = unpack_poweron(buf[0]);
                powerondata poweron_stuct = {untransform_poweron(poweron_d)}; // TODO: rename the var

                // Print all the data to serial so the parser can read it and send it to the db and dashboard
                Serial.println(poweron_stuct.curr_state);
                Serial.println(poweron_stuct.external_temp);
                Serial.println(poweron_stuct.temperature_engbay);
                Serial.println(poweron_stuct.temperature_avbay);
                Serial.println(poweron_stuct.gps_quality);
                Serial.println(poweron_stuct.gps_fix);
                Serial.println(poweron_stuct.gps_num_satellites);
                Serial.println(poweron_stuct.gps_antenna_status);
                Serial.println(poweron_stuct.temperature_avbay);
                char failures_bits_str[17];
                uint16_to_binary_string(poweron_stuct.failures, &failures_bits_str);
                Serial.println(failures_bits_str);

                // Psuedo code for two-way comms
                // if (received signal from serial) {
                //     // transmit the signal to switch states
                //     transmit(rf96, state_switch_signal, 1);
                // }
            }
            case state::LAUNCH_READY:
            {
                unsigned int *launchready_d = unpack_launchready(buf[0]);
                launchreadydata launchready_stuct = untransform_launchready(launchready_d); // TODO: rename the var

                // Print all the data to serial so the parser can read it and send it to the db and dashboard
                Serial.println(launchready_stuct.curr_state);
                Serial.println(launchready_stuct.gps_num_satellites);
                Serial.println(launchready_stuct.gps_long);
                Serial.println(launchready_stuct.gps_lat);
                Serial.println(launchready_stuct.gyro_x);
                Serial.println(launchready_stuct.accel_y);
                Serial.println(launchready_stuct.gyro_y);
                Serial.println(launchready_stuct.vert_velo);
                Serial.println(launchready_stuct.gyro_z);

                Serial.println(launchready_stuct.accel_x);
                Serial.println(launchready_stuct.altitude);
                Serial.println(launchready_stuct.gps_fix);
                Serial.println(launchready_stuct.external_temp);
                Serial.println(launchready_stuct.temperature_avbay);
                Serial.println(launchready_stuct.accel_z);
                Serial.println(launchready_stuct.mag_y);
                Serial.println(launchready_stuct.mag_z);

                char failures_bits_str[17];
                uint16_to_binary_string(launchready_stuct.failures, &failures_bits_str);
                Serial.println(failures_bits_str);

                Serial.println(launchready_stuct.gps_speed);
                Serial.println(launchready_stuct.gps_altitude);
                Serial.println(launchready_stuct.gps_quality);
                Serial.println(launchready_stuct.temperature_engbay);
                Serial.println(launchready_stuct.gps_antenna_status);

                // Psuedo code for two-way comms
                // if (received signal from serial) {
                //     // transmit the signal to switch states
                //     transmit(rf96, state_switch_signal, 1);
                // }
            }
            case state::LAUNCH_READY:
            {
                unsigned int *launchmode_d = unpack_launchmode(buf[0]);
                launchmodedata launchmode_stuct = unpack_launchmode(launchmode_d); // TODO: rename the var

                // Print all the data to serial so the parser can read it and send it to the db and dashboard
                Serial.println(launchready_stuct.curr_state);
                Serial.println(launchready_stuct.gps_num_satellites);
                Serial.println(launchready_stuct.gps_long);
                Serial.println(launchready_stuct.gps_lat);
                Serial.println(launchready_stuct.gyro_x);
                Serial.println(launchready_stuct.accel_y);
                Serial.println(launchready_stuct.gyro_y);
                Serial.println(launchready_stuct.vert_velo);
                Serial.println(launchready_stuct.gyro_z);

                Serial.println(launchready_stuct.accel_x);
                Serial.println(launchready_stuct.altitude);
                Serial.println(launchready_stuct.gps_fix);
                Serial.println(launchready_stuct.external_temp);
                Serial.println(launchready_stuct.temperature_avbay);
                Serial.println(launchready_stuct.accel_z);
                Serial.println(launchready_stuct.mag_y);
                Serial.println(launchready_stuct.mag_z);

                char failures_bits_str[17];
                uint16_to_binary_string(launchready_stuct.failures, &failures_bits_str);
                Serial.println(failures_bits_str);

                Serial.println(launchready_stuct.gps_speed);
                Serial.println(launchready_stuct.gps_altitude);
                Serial.println(launchready_stuct.gps_quality);
                Serial.println(launchready_stuct.temperature_engbay);
                Serial.println(launchready_stuct.gps_antenna_status);

                // Psuedo code for two-way comms
                // if (received signal from serial) {
                //     // transmit the signal to switch states
                //     transmit(rf96, state_switch_signal, 1);
                // }
            }
            default:
                // code block
                break;
            }
        }
        else
        {
            Serial.println("Receive failed");
        }
    }
    else
    {
        Serial.println("No reply, is there a listener around?");
    }
}

void uint16_to_binary_string(uint16_t num, char *result)
{
    for (int i = 15; i >= 0; i--)
    {
        result[15 - i] = ((num & (1 << i)) ? '1' : '0');
    }
    result[16] = '\0'; // Null-terminate the string
}