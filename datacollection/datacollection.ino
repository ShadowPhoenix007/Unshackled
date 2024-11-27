#define flex1 A0
#define flex2 A1
#define flex3 A2
#define flex4 A3
#define piezo A6
#define flex5 A7


#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

float acceleration[3] = {};
float gyro[3] = {};
float temperature = 0;


  int flex1i = 0;
  int flex2i = 0;
   int flex3i = 0;
   int flex4i = 0;
   int flex5i = 0;
   int piezoi;
   int flexA[5] = {0,0,0,0,0};
   int flexMax[5] = {0,0,0,0,0};
   int flexMin[5] = {0,0,0,0,0};

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);

  
  Serial.begin(115200);
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G); //could be 2,4,8, or 16
  mpu.setGyroRange(MPU6050_RANGE_500_DEG); //could be 250, 500, 1000, or 2000
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); //could be 260, 184, 94, 44, 21, 10, or 5

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
  printiArray(flexA, 5, false);
  printfArray(acceleration, 3, false);
  printfArray(gyro, 3, false);
  Serial.print(temperature);
  Serial.println();
  delay(100);
}

void Read()
{ 
  flexA[0] = analogRead(flex1);
  flexA[1] = analogRead(flex2);
  flexA[2] = analogRead(flex3);
  flexA[3] = analogRead(flex4);
  flexA[4] = analogRead(flex5);
  
  piezoi = analogRead(piezo);

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);


  acceleration[0] = a.acceleration.x; //m/s^2
  acceleration[1] = a.acceleration.y;
  acceleration[2] = a.acceleration.z;

  gyro[0] = g.gyro.x; //rad/s
  gyro[1] = g.gyro.y;
  gyro[2] = g.gyro.z;

  temperature = temp.temperature;

}

