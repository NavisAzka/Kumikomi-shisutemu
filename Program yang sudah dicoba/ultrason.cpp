#include <Arduino.h>

#define TRIG_PIN 5
#define ECHO_PIN 18

float readDistanceCM()
{
    // Pastikan TRIG LOW
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    // Kirim pulsa 10 us
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Baca durasi pantulan
    long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30 ms

    if (duration == 0)
        return -1.0; // Tidak ada objek

    // Konversi ke cm
    return duration * 0.0343 / 2.0;
}

void setup()
{
    Serial.begin(115200);

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    digitalWrite(TRIG_PIN, LOW);
}

void loop()
{
    float distance = readDistanceCM();

    if (distance < 0)
    {
        Serial.println("Tidak ada objek");
    }
    else
    {
        Serial.print("Jarak: ");
        Serial.print(distance);
        Serial.println(" cm");
    }

    delay(100);
}