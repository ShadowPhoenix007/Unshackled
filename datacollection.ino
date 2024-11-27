#define flex1 A0
#define flex2 A1
#define flex3 A2
#define flex4 A3
#define piezo A6
#define flex5 A7

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
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  Serial.println("Flex in max positions in ");
  delay(1000);
  count(3);
  Serial.println("FLEX!");
  Read();  
  copyArray(flexA, flexMax, 5);
  printArray(flexMax, 5);
  Serial.println("Flex in min positions in ");
  delay(1000);
  count(3);
  Serial.println("FLEX!");
  Read();
  copyArray(flexA, flexMin, 5);
  printArray(flexMin, 5);
  Serial.print("flex1, flex2, flex3, flex4, flex5, piezo");
}

void copyArray(int source[], int destination[], int size) {
  for (int i = 0; i < size; i++) 
  {
    destination[i] = source[i];
  }
}

void printArray(int arr[], int size) {
  for (int i = 0; i < size; i++) {
    Serial.print(arr[i]);
    if (i < size - 1) {
      Serial.print(", ");
    }
  }
  Serial.println();
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
  printArray(flexA, 5);
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
}

