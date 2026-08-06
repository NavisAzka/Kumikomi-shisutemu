#include <Arduino.h>
#include <SPI.h>

//=================== PIN SPI ===================
#define PIN_CS    5
#define PIN_SCK   18
#define PIN_MISO  19
#define PIN_MOSI  23

//=================== REGISTER ==================
#define WHO_AM_I      0x75
#define PWR_MGMT_1    0x6B

#define ACCEL_XOUT_H  0x3B
#define GYRO_XOUT_H   0x43

SPIClass spi(VSPI);

//===============================================

void writeRegister(uint8_t reg, uint8_t data)
{
    digitalWrite(PIN_CS, LOW);

    spi.transfer(reg);
    spi.transfer(data);

    digitalWrite(PIN_CS, HIGH);
}

uint8_t readRegister(uint8_t reg)
{
    digitalWrite(PIN_CS, LOW);

    spi.transfer(reg | 0x80);

    uint8_t value = spi.transfer(0);

    digitalWrite(PIN_CS, HIGH);

    return value;
}

int16_t read16(uint8_t reg)
{
    digitalWrite(PIN_CS, LOW);

    spi.transfer(reg | 0x80);

    uint8_t high = spi.transfer(0);
    uint8_t low  = spi.transfer(0);

    digitalWrite(PIN_CS, HIGH);

    return (int16_t)((high << 8) | low);
}

void setup()
{
    Serial.begin(115200);

    pinMode(PIN_CS, OUTPUT);
    digitalWrite(PIN_CS, HIGH);

    spi.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CS);

    delay(100);

    // Wake up MPU6500
    writeRegister(PWR_MGMT_1, 0x00);

    delay(100);

    Serial.print("WHO_AM_I = 0x");
    Serial.println(readRegister(WHO_AM_I), HEX);
}

void loop()
{
    int16_t ax = read16(ACCEL_XOUT_H);
    int16_t ay = read16(ACCEL_XOUT_H + 2);
    int16_t az = read16(ACCEL_XOUT_H + 4);

    int16_t gx = read16(GYRO_XOUT_H);
    int16_t gy = read16(GYRO_XOUT_H + 2);
    int16_t gz = read16(GYRO_XOUT_H + 4);

    float AccX = ax / 16384.0;
    float AccY = ay / 16384.0;
    float AccZ = az / 16384.0;

    float GyroX = gx / 131.0;
    float GyroY = gy / 131.0;
    float GyroZ = gz / 131.0;

    Serial.print("ACC : ");
    Serial.print(AccX);
    Serial.print(", ");
    Serial.print(AccY);
    Serial.print(", ");
    Serial.print(AccZ);

    Serial.print("   GYRO : ");

    Serial.print(GyroX);
    Serial.print(", ");
    Serial.print(GyroY);
    Serial.print(", ");
    Serial.println(GyroZ);

    delay(10);
}