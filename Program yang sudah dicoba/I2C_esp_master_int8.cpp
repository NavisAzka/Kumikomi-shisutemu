#include <Arduino.h>
#include <Wire.h>

#define SLAVE_ADDR 0x08
int value = 0;

void setup() {
    Serial.begin(115200);

    Wire.begin();              // SDA=21 SCL=22

    Serial.println("ESP32 I2C Master");
}

void loop() {

    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write((uint8_t *)&value, sizeof(value));
    Wire.endTransmission();

    Serial.print("Kirim: ");
    Serial.println(value);

    value++;

    delay(1000);
}