#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS1.h>
Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();
void setupSensor() {
    if (!lsm.begin()) {
        Serial.println("Failed to communicate with LSM9DS1.");
        while (1);
    }
    lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_2G);
    lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
    lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);
}
void setup() {
  Serial.begin(115200);
  setupSensor();
}
void loop() {
    lsm.read();
    sensors_event_t a, m, g, temp;
    lsm.getEvent(&a, &m, &g, &temp);
    Serial.print("Accel X: "); Serial.print(a.acceleration.x);
    Serial.print(" Y: "); Serial.print(a.acceleration.y);
    Serial.print(" Z: "); Serial.println(a.acceleration.z);

    Serial.print("Mag X: "); Serial.print(m.magnetic.x);
    Serial.print(" Y: "); Serial.print(m.magnetic.y);
    Serial.print(" Z: "); Serial.println(m.magnetic.z);
    
    Serial.print("Gyro X: "); Serial.print(g.gyro.x);
    Serial.print(" Y: "); Serial.print(g.gyro.y);
    Serial.print(" Z: "); Serial.println(g.gyro.z);
    delay(1000);
}