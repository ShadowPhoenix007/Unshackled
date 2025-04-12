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

float acceleration[3] = {};
float gyro[3] = {};
float temperature = 0;

float piezoi = 0;
int flexA[5] = {0, 0, 0, 0, 0};
int flexMax[5] = {0, 0, 0, 0, 0};
int flexMin[5] = {0, 0, 0, 0, 0};

// PID variables
float accelLastError[3] = {0, 0, 0};
float gyroLastError[3] = {0, 0, 0};
float accelIntegral[3] = {0, 0, 0};
float gyroIntegral[3] = {0, 0, 0};

float accelPID[3] = {0, 0, 0};
float gyroPID[3] = {0, 0, 0};

String inputString = "";

// PID Constants
float kp = 0;
float ki = 0;
float kd = 0;
float deadZone = 10; // Small tolerance to prevent unnecessary corrections

void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(flex1, INPUT);
  pinMode(flex2, INPUT);
  pinMode(flex3, INPUT);
  pinMode(flex4, INPUT);
  pinMode(flex5, INPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);

  mpu.initialize();
  if (mpu.testConnection()) {
    Serial.println("MPU6050 connection successful!");
  } else {
    Serial.println("MPU6050 connection failed!");
  }
}

void loop() {
  Read();
  TremorDamper();
  
  // Print PID values
  printfArray(accelPID, 3, false);
  printfArray(gyroPID, 3, true);
  delay(100);
}

float PID(float setpoint, float measurement, float& integral, float& lastError) {
    float error = setpoint - measurement;

    if (abs(error) < deadZone) {
        integral = 0;  // Reset integral if error is too small
    } else {
        integral += error;
    }

    float proportional = kp * error;
    float derivative = kd * (error - lastError);
    lastError = error;

    return (measurement - (proportional + (ki * integral) + derivative));
}

void Read() {
  flexA[0] = analogRead(flex1);
  flexA[1] = analogRead(flex2);
  flexA[2] = analogRead(flex3);
  flexA[3] = analogRead(flex4);
  flexA[4] = analogRead(flex5);

  piezoi = analogRead(piezo);

  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Apply calibration offsets
  acceleration[0] = ax ;
  acceleration[1] = ay ;
  acceleration[2] = az ;

  gyro[0] = gx;
  gyro[1] = gy;
  gyro[2] = gz;

  // PID for acceleration
  for (int i = 0; i < 3; i++) {
    accelPID[i] = PID(0, acceleration[i], accelIntegral[i], accelLastError[i]);
  }

  // PID for gyroscope data
  for (int i = 3; i < 6; i++) {
    gyroPID[i - 3] = PID(0, gyro[i - 3], gyroIntegral[i - 3], gyroLastError[i - 3]);
  }
}

void TremorDamper() {
  if (Serial.available()) {
    inputString = Serial.readStringUntil('\n'); // Read command

    if (inputString == "AMotorFOR") {
      RunMotorA(0.8);
    } 
    else if (inputString == "AMotorREV") {
      RunMotorA(-0.8);
    }
    else if (inputString == "AMotorOff") {
      RunMotorA(0);
    }

    if (inputString == "BMotorFOR") {
      RunMotorB(0.8);
    } 
    else if (inputString == "BMotorREV") {
      RunMotorB(-0.8);
    }
    else if (inputString == "BMotorOff") {
      RunMotorB(0);
    }
  }
}

void RunMotorA(double powerA) {
  if (powerA > 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, powerA * 255);
  } else if (powerA < 0) {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    analogWrite(PWMA, -powerA * 255);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, 0);
  }
}

void RunMotorB(double powerB) {
  if (powerB > 0) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMB, powerB * 255);
  } else if (powerB < 0) {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    analogWrite(PWMB, -powerB * 255);
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMB, 0);
  }
}

void printfArray(float arr[], int size, boolean line) {
  for (int i = 0; i < size; i++) {
    Serial.print(arr[i]);
    if (i < size - 1) {
      Serial.print(", ");
    }
  }
  if (line) {
    Serial.println();
  } else {
    Serial.print(", ");
  }
}
