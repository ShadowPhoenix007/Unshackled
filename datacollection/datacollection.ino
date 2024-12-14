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

float acceleration[3] = {};
float gyro[3] = {};
float temperature = 0;


   int piezoi;
   int flexA[5] = {0,0,0,0,0};
   int flexMax[5] = {0,0,0,0,0};
   int flexMin[5] = {0,0,0,0,0};

   float accelError[3] = {0, 0, 0};
float gyroError[3] = {0, 0, 0};
float accelLastError[3] = {0, 0, 0};
float gyroLastError[3] = {0, 0, 0};
float accelIntegral[3] = {0, 0, 0};
float gyroIntegral[3] = {0, 0, 0};

//pid
float accelPID[3] = {0, 0, 0};
float gyroPID[3] = {0, 0, 0};

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(115200);

    Wire.begin();
  Serial.begin(115200);
  Serial.println("Initializing MPU6050...");
  mpu.initialize();
  if (mpu.testConnection()) {
    Serial.println("MPU6050 connection successful!");
  } else {
    Serial.println("MPU6050 connection failed!");
  }

  Serial.println("MPU6050 Found!");

  Serial.println("Flex in max positions in ");
  delay(1000);
  count(3);
  Serial.println("FLEX!");
  Read();  
  copyArray(flexA, flexMax, 5);
  printiArray(flexMax, 5, true);
  Serial.println("Flex in min positions in ");
  delay(1000);
  count(3);
  Serial.println("FLEX!");
  Read();
  copyArray(flexA, flexMin, 5);
  printiArray(flexMin, 5, true);
  Serial.print("flex1, flex2, flex3, flex4, flex5, piezo");
}

void copyArray(int source[], int destination[], int size) {
  for (int i = 0; i < size; i++) 
  {
    destination[i] = source[i];
  }
}

void printiArray(int arr[], int size, boolean line) {
  for (int i = 0; i < size; i++) {
    Serial.print(arr[i]);
    if (i < size - 1) {
      Serial.print(", ");
    }
  }
  if (line==true)
  {
    Serial.println();
  }
  else
  {
    Serial.print(", ");
  }
}

void printfArray(float arr[], int size, boolean line) {
  for (int i = 0; i < size; i++) {
    Serial.print(arr[i]);
    if (i < size - 1) {
      Serial.print(", ");
    }
  }
  if (line==true)
  {
    Serial.println();
  }
  else
  {
    Serial.print(", ");
  }
}

void count(int counter)
{
  for (int i = counter; i > 0; i--)
  {
    Serial.print(i);
    delay(1000);
  }
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:

  Read();
  // // print out the value you read:
  //printiArray(flexA, 5, false);
  printfArray(accelPID, 3, false);
  printfArray(gyroPID, 3, true);
  Serial.println();
  delay(100);
}

float kp = 25;
float ki = 0.1;
float kd = 15;
float PID(float setpoint, float measurement, float& integral, float& lastError)
{
  float error = setpoint - measurement;
  float proportional = kp * error;
  float derivative = kd * (error - lastError);
  lastError = error;
  integral = integral + error;
  float finIntegral = integral * ki;
  return (proportional + finIntegral + derivative);
}

void Read()
{
  flexA[0] = analogRead(flex1);
  flexA[1] = analogRead(flex2);
  flexA[2] = analogRead(flex3);
  flexA[3] = analogRead(flex4);
  flexA[4] = analogRead(flex5);
  
  piezoi = analogRead(piezo);

  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  acceleration[0] = ax; //m/s^2
  acceleration[1] = ay;
  acceleration[2] = az;

  gyro[0] = gx; //rad/s
  gyro[1] = gy;
  gyro[2] = gz;

  for (int i = 0; i < 3; i++) {
    accelPID[i] = PID(0, acceleration[i], accelLastError[i], accelIntegral[i]);
  }

  // Apply PID to gyroscope data
  for (int i = 0; i < 3; i++) {
    gyroPID[i] = PID(0, gyro[i], gyroLastError[i], gyroIntegral[i]);
  }
}

