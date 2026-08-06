#include <Arduino.h>

HardwareSerial UART(2);

void setup() {
    Serial.begin(115200);
    UART.begin(115200, SERIAL_8N1, 16, 17); // RX, TX

    Serial.println("ESP32 Transmitter");
}

void loop() {

    UART.println("Hello ESP32!");

    Serial.println("Data terkirim");

    delay(1000);
}