#include <Wire.h>
#include <Arduino.h>

#define MPU6050_ADDR 0x68

#define PWR_MGMT_1   0x6B
#define WHO_AM_I     0x75

#define ACCEL_XOUT_H 0x3B
#define GYRO_XOUT_H  0x43

class Kalman
{
private:
    float Q = 0.001;
    float R = 0.1;
    float X = 0;
    float P = 1;
    float K = 0;

public:
    float update(float measurement)
    {
        P += Q;
        K = P / (P + R);
        X = X + K * (measurement - X);
        P = (1 - K) * P;

        return X;
    }
};

Kalman axFilter;
Kalman ayFilter;
Kalman azFilter;

Kalman gxFilter;
Kalman gyFilter;
Kalman gzFilter;


void writeRegister(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

uint8_t readRegister(uint8_t reg)
{
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);

    Wire.requestFrom(MPU6050_ADDR, 1);

    return Wire.read();
}

int16_t read16(uint8_t reg)
{
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);

    Wire.requestFrom(MPU6050_ADDR, 2);

    uint8_t high = Wire.read();
    uint8_t low  = Wire.read();

    return (high << 8) | low;
}

void setup()
{
    Serial.begin(115200);

    Wire.begin(21,22);

    delay(100);

    // Wake up MPU6050
    writeRegister(PWR_MGMT_1,0x00);

    delay(100);

    Serial.print("WHO_AM_I = 0x");
    Serial.println(readRegister(WHO_AM_I),HEX);
}

void loop()
{
    int16_t ax = read16(ACCEL_XOUT_H);
    int16_t ay = read16(ACCEL_XOUT_H + 2);
    int16_t az = read16(ACCEL_XOUT_H + 4);

    int16_t gx = read16(GYRO_XOUT_H);
    int16_t gy = read16(GYRO_XOUT_H + 2);
    int16_t gz = read16(GYRO_XOUT_H + 4);

    float rawAccX = ax / 16384.0;
    float rawAccY = ay / 16384.0;
    float rawAccZ = az / 16384.0;

    float AccX = axFilter.update(rawAccX);
    float AccY = ayFilter.update(rawAccY);
    float AccZ = azFilter.update(rawAccZ);


    Serial.print(">");

    Serial.print("X:");
    Serial.print(AccX);
    Serial.print(",");

    Serial.print("Y:");
    Serial.print(AccY);
    Serial.print(",");

    Serial.print("Z:");
    Serial.print(AccZ);
    Serial.print(",");

    Serial.print("rawX:");
    Serial.print(rawAccX);
    Serial.print(",");

    Serial.print("rawY:");
    Serial.print(rawAccY);
    Serial.print(",");      

    Serial.print("rawZ:");
    Serial.print(rawAccZ);
    Serial.print(",");

    Serial.println("-");

    delay(10);
}