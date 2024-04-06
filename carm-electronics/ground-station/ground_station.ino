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

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Initializing revelant objects and structs
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Singleton instance of the radio driver
RH_RF95 rf96(RFM96_CS, RFM96_INT);

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
            state curr_state = get_state_from_bits(buf[0]);
            switch (curr_state)
            {
            case state::POWER_ON:
            {
                unsigned int *poweron_d = unpack_poweron(buf[0]);
                uint64_t[1] poweron_word = {pack_poweron(poweron_d)};
                transmit(rf96, poweron_word, 1);
                receive(rf96, 1) break;
            }
            case state::LAUNCH_READY:
            {
                unsigned int *launchready_d = transform_launchready(bboard_manager);
                uint64_t *launchready_words = pack_launchready(launchready_d);
                transmit(rf96, launchready_words, 5);
                receive(rf96, 1);
                break;
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