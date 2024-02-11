
/**************************************************************************/
/*!
This is a demo for the Adafruit MCP9808 breakout
----> http://www.adafruit.com/products/1782
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!
*/
/**************************************************************************/

#include <Wire.h>
#include "bitpack.c"

void setup()
{
    Serial.begin(9600);
    while (!Serial)
        ; // waits for serial terminal to be open, necessary in newer arduino boards.
}

void loop()
{

    uint32_t temp = 0;
    Serial.println(c);
    uint32_t test = 22;
    Serial.println(test);
    uint32_t bitpacked_word = Bitpack_newu(temp, 5, 0, test);
    int unbitpacked_word = Bitpack_getu(bitpacked_word, 5, 0);

    // printing out the temp values that were bitpacked
    Serial.print("Temp: ");
    Serial.println(unbitpacked_word);
}
