#include <Arduino.h>
#include <Wire.h>

#define SLAVE_ADDR 0x08


void receiveEvent(int len)
{
    int value;

    Wire.readBytes((char *)&value, sizeof(value));

    Serial.print("Terima: ");
    Serial.println(value);
}

void setup()
{
    Serial.begin(115200);

    Wire.begin((uint8_t)SLAVE_ADDR);

    Wire.onReceive(receiveEvent);

    Serial.println("ESP32 I2C Slave");
}

void loop()
{
}