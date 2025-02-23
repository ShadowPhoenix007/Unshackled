#define PWMA 6
#define AIN1 8
#define AIN2 7
#define PWMB 5
#define BIN1 4
#define BIN2 3

#define flex1 A0
#define flex2 A1
#define flex3 A2
#define flex4 A3
#define piezo A7
#define flex5 A6


#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

MPU6050 mpu;

String inputString = ""; 

void loop()
{
  if (Serial.available()) {
        inputString = Serial.readStringUntil('\n'); // Read command

        if (inputString == "AMotorFOR") {
            RunMotorA(0.8);
        } 
        else if (inputString == "AMotorREV")
        {
          RunMotorA(-0.8);
        }
        else if (inputString == "AMotorOff") {
            RunMotorA(0);
        }
    }
}
void setup()
{
  Serial.begin(115200);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
}
void RunMotorA(double powerA)
{
  if (powerA>0)
  {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, powerA*255);
  }
  else if (powerA<0)
  {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    analogWrite(PWMA, -powerA*255);
  }
  else
  {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, 0);
  }
}

void RunMotorB(double powerB)
{
  if (powerB>0)
  {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMB, powerB*255);
  }
  else if (powerB<0)
  {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    analogWrite(PWMB, powerB*255);
  }
  else
  {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMB, 0);
  }

}