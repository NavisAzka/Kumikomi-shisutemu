#include <Arduino.h>
#include <Wire.h>

#define MPU_ADDR 0x68

#define PWR_MGMT_1 0x6B
#define GYRO_CONFIG 0x1B
#define GYRO_ZOUT_H 0x47

float gyroBiasZ = 0.0;
float yaw = 0.0;

uint32_t lastTime;

//============================

void writeRegister(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

int16_t read16(uint8_t reg)
{
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);

    Wire.requestFrom(MPU_ADDR, 2);

    uint8_t high = Wire.read();
    uint8_t low  = Wire.read();

    return (int16_t)((high << 8) | low);
}

//============================

void calibrateGyro()
{
    Serial.println("Kalibrasi gyro...");
    Serial.println("JANGAN GERAKKAN SENSOR!");

    const int N = 2000;

    double sum = 0;

    for(int i=0;i<N;i++)
    {
        int16_t gz = read16(GYRO_ZOUT_H);

        sum += gz / 131.0;

        delay(2);
    }

    gyroBiasZ = sum / N;

    Serial.print("Bias Z = ");
    Serial.println(gyroBiasZ,6);
}

//============================

void setup()
{
    Serial.begin(115200);

    Wire.begin(21,22);

    writeRegister(PWR_MGMT_1,0x00);

    // Gyro ±250 dps
    writeRegister(GYRO_CONFIG,0x00);

    delay(200);

    calibrateGyro();

    lastTime = micros();
}

//============================

void loop()
{
    uint32_t now = micros();

    float dt = (now - lastTime) * 1e-6;

    lastTime = now;

    //--------------------

    int16_t rawGz = read16(GYRO_ZOUT_H);

    float gyroZ = rawGz / 131.0;

    gyroZ -= gyroBiasZ;

    //--------------------

    yaw += gyroZ * dt;

    // Batasi menjadi 0-360°
    while(yaw >= 360.0) yaw -= 360.0;
    while(yaw < 0.0) yaw += 360.0;

    //--------------------

    Serial.print(">");

    Serial.print("Yaw:");
    Serial.print(yaw,2);

    Serial.print(",");

    Serial.print("GZ:");
    Serial.print(gyroZ,2);

    Serial.print(",");

    Serial.print("dt:");
    Serial.print(dt,6);

    Serial.println("-");
}