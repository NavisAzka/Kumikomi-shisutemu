#include <Arduino.h>
#include <Wire.h>

#define SLAVE_ADDR 0x08

void setup() {
    Serial.begin(115200);

    Wire.begin();              // SDA=21 SCL=22

    Serial.println("ESP32 I2C Master");
}

void loop() {

    Wire.beginTransmission(SLAVE_ADDR);

    Wire.write("Hello ESP32");

    Wire.endTransmission();

    Serial.println("Data terkirim");

    delay(1000);
}