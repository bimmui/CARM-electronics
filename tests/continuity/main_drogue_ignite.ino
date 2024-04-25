const int MAIN = 15;
const int DROGUE = 16;

void setup()
{
    pinMode(MAIN, OUTPUT);
    pinMode(DROGUE, OUTPUT);
}

void loop()
{
    Serial.println("Testing the drogue pins... ");
    delay(5000);
    digitalWriteln(DROGUE, HIGH);
    delay(500);
    digitalWrite(DROGUE, LOW);
    Serial.print("Testing the main pins... ");
    delay(5000);
    digitalWrite(MAIN, HIGH);
    delay(500);
    digitalWrite(MAIN, LOW);
}