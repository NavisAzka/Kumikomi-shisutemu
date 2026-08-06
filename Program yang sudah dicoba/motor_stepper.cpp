#define STEP_PIN 18
#define EN_PIN   19
#define RESET_PIN 21

void move90()
{
    for(int i=0;i<50;i++)
    {
        digitalWrite(STEP_PIN,HIGH);
        delayMicroseconds(1000);

        digitalWrite(STEP_PIN,LOW);
        delayMicroseconds(1000);
    }
}

void setup()
{
    pinMode(STEP_PIN,OUTPUT);
    pinMode(EN_PIN,OUTPUT);
    pinMode(RESET_PIN,OUTPUT);

    digitalWrite(EN_PIN,HIGH);
    digitalWrite(RESET_PIN,HIGH);
}

void loop()
{
    move90();
    delay(2000);
}