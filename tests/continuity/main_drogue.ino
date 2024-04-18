const int MAIN = 15;
const int DROGUE = 16;

void setup()
{
    pinMode(MAIN, INPUT);
    pinMode(DROGUE, INPUT);
}

void loop()
{
    Serial.print("Drogue: ");
    Serial.println(digitalRead(DROGUE));
    Serial.print("Main: ");
    Serial.println(digitalRead(MAIN));
    delay(500);
}