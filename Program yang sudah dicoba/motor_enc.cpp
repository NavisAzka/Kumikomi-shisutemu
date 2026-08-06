#include <Arduino.h>

//==================== MOTOR ====================
#define ENA 25
#define IN1 26
#define IN2 27

//==================== ENCODER ==================
#define ENCODER_PIN 34

volatile long encoderCount = 0;

// PWM
const int pwmChannel = 0;
const int pwmFreq = 1000;
const int pwmResolution = 8;

// Encoder
const int PPR = 20;      // Sesuaikan dengan encoder Anda

unsigned long previousMillis = 0;
int motorSpeed = 150;

//================================================

void IRAM_ATTR encoderISR()
{
    encoderCount++;
}

//================================================

void motorForward(uint8_t speed)
{
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    ledcWrite(pwmChannel, speed);
}

void motorBackward(uint8_t speed)
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);

    ledcWrite(pwmChannel, speed);
}

void motorStop()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    ledcWrite(pwmChannel, 0);
}

//================================================

void setup()
{
    Serial.begin(115200);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    pinMode(ENCODER_PIN, INPUT_PULLUP);

    // PWM
    ledcSetup(pwmChannel, pwmFreq, pwmResolution);
    ledcAttachPin(ENA, pwmChannel);

    // Interrupt Encoder
    attachInterrupt(
        digitalPinToInterrupt(ENCODER_PIN),
        encoderISR,
        FALLING);

    Serial.println("--------------------------------");
    Serial.println("ESP32 Motor + Encoder Test");
    Serial.println("--------------------------------");
    Serial.println("Perintah:");
    Serial.println("F = Forward");
    Serial.println("B = Backward");
    Serial.println("S = Stop");
    Serial.println("0-255 = PWM");
    Serial.println("--------------------------------");

    motorForward(motorSpeed);
}

//================================================

void loop()
{
    // ==================== Serial Command ====================

    if (Serial.available())
    {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();

        if (cmd == "F")
        {
            motorForward(motorSpeed);
            Serial.println("Forward");
        }

        else if (cmd == "B")
        {
            motorBackward(motorSpeed);
            Serial.println("Backward");
        }

        else if (cmd == "S")
        {
            motorStop();
            Serial.println("Stop");
        }

        else
        {
            int pwm = cmd.toInt();

            if (pwm >= 0 && pwm <= 255)
            {
                motorSpeed = pwm;
                motorForward(motorSpeed);

                Serial.print("PWM = ");
                Serial.println(motorSpeed);
            }
        }
    }

    // ==================== RPM ====================

    if (millis() - previousMillis >= 1000)
    {
        previousMillis = millis();

        noInterrupts();
        long pulse = encoderCount;
        encoderCount = 0;
        interrupts();

        float rpm = (pulse * 60.0) / PPR;

        Serial.print("Pulse : ");
        Serial.print(pulse);

        Serial.print("\tRPM : ");
        Serial.print(rpm);

        Serial.print("\tPWM : ");
        Serial.println(motorSpeed);
    }
}