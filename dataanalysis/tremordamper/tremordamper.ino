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
#include <arduinoFFT.h>

#define SAMPLES 16
#define SAMPLING_FREQUENCY 50 // Hz

MPU6050 mpu;

// FFT setup
ArduinoFFT<float> FFT = ArduinoFFT<float>();
float accelXReal[SAMPLES], accelXImag[SAMPLES];
float accelYReal[SAMPLES], accelYImag[SAMPLES];
float gyroXReal[SAMPLES], gyroXImag[SAMPLES];
float gyroYReal[SAMPLES], gyroYImag[SAMPLES];

float piezoi = 0;
int flexA[5] = {0, 0, 0, 0, 0};
int flexMax[5] = {0, 0, 0, 0, 0};
int flexMin[5] = {0, 0, 0, 0, 0};

// Motor power outputs and targets
float motorPowerX = 0;
float motorPowerY = 0;
float motorTargetX = 0;
float motorTargetY = 0;

// PID parameters
float kp = 0.4, ki = 0.0, kd = 0.05;
float deadZone = 2.0;
float accelIntegral[2] = {0, 0};
float accelLastError[2] = {0, 0};

String inputString = "";

// Tremor detection threshold (tune this)
float threshold = 100.0;

void setup() {
  Serial.begin(115200);
  Serial.println("SETUP START");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);

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
  ReadFlexAndPiezo();
  collectSensorData();
  bool tremorX = detectTremor(accelXReal, gyroXReal, "X");
  bool tremorY = detectTremor(accelYReal, gyroYReal, "Y");

  // PID adjustment for smooth damping
  motorPowerX = PID(motorTargetX, motorPowerX, accelIntegral[0], accelLastError[0]);
  motorPowerY = PID(motorTargetY, motorPowerY, accelIntegral[1], accelLastError[1]);

  if (tremorX) RunMotorA(motorPowerX); else RunMotorA(0);
  if (tremorY) RunMotorB(motorPowerY); else RunMotorB(0);
  delay(10);
}

void ReadFlexAndPiezo() {
  flexA[0] = analogRead(flex1);
  flexA[1] = analogRead(flex2);
  flexA[2] = analogRead(flex3);
  flexA[3] = analogRead(flex4);
  flexA[4] = analogRead(flex5);
  piezoi = analogRead(piezo);
}

void collectSensorData() {
  for (int i = 0; i < SAMPLES; i++) {
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    float ax_ms2 = ax / 16384.0 * 9.81;
    float ay_ms2 = ay / 16384.0 * 9.81;
    float az_ms2 = az / 16384.0 * 9.81;

    float gx_dps = gx / 131.0;
    float gy_dps = gy / 131.0;
    float gz_dps = gz / 131.0;
    accelXReal[i] = ax_ms2;
    accelYReal[i] = ay_ms2;
    gyroXReal[i] = gx_dps;
    gyroYReal[i] = gy_dps;

    accelXImag[i] = accelYImag[i] = 0;
    gyroXImag[i] = gyroYImag[i] = 0;

    delay(1000 / SAMPLING_FREQUENCY);
  }
}

bool detectTremor(float real[], float real2[], String axis) {
  float imag[SAMPLES] = {0};
  float imag2[SAMPLES] = {0};

  FFT.windowing(real, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(real, imag, SAMPLES, FFT_FORWARD);
  FFT.complexToMagnitude(real, imag, SAMPLES);

  FFT.windowing(real2, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(real2, imag2, SAMPLES, FFT_FORWARD);
  FFT.complexToMagnitude(real2, imag2, SAMPLES);

  float power = 0;
  for (int i = 1; i < SAMPLES / 2; i++) {
    float freq = (i * SAMPLING_FREQUENCY) / SAMPLES;
    if (freq >= 4 && freq <= 12) {
      float mag = (real[i] + real2[i]) / 2.0;
      if (mag > power) power = mag;
    }
  }

  float mappedPower = constrain(power / 600.0, 0, 1.0);
  if (axis == "X") motorTargetX = mappedPower;
  if (axis == "Y") motorTargetY = mappedPower;

  Serial.println(axis + String(power));

  return (power > threshold);
}

void RunMotorA(float powerA) {
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

void RunMotorB(float powerB) {
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

float PID(float setpoint, float measurement, float& integral, float& lastError) {
  float error = setpoint - measurement;

  if (abs(error) < deadZone) {
    integral = 0;
  } else {
    integral += error;
  }

  float proportional = kp * error;
  float derivative = kd * (error - lastError);
  lastError = error;

  return (measurement + proportional + (ki * integral) + derivative);
}

void printfArray(float* arr, int size, const char* label) {
  Serial.print(label);
  for (int i = 0; i < size; i++) {
    Serial.print(arr[i]);
    Serial.print(", ");
  }
  Serial.println();
}