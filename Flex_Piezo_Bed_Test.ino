#include <SPI.h>
#include <SD.h>

const int piezoPin = A0;
const int piezoPin2 = A1;
const int flexPin = A2;

unsigned long startTime;
const unsigned long duration = 90000;

int chipSelect = 10; //chip select pin for the MicroSD Card Adapter
File file; // file object that is used to read and write data
String filename = "FBT5.csv";

void setup() {
  Serial.begin(9600);
  
  pinMode(chipSelect, OUTPUT);

  if(!SD.begin(chipSelect)){
    Serial.println("SD card not found");
    return;
  }
  Serial.println("SD card found!");

  file = SD.open(filename, FILE_WRITE);
  if(file){
    file.println("Timestamp (ms), Piezo Value Left, Piezo Value Right, Flex Value Middle,  Label");
    file.close();
  } else{ 
    Serial.println("Failed to open file"); 
    }

  startTime = millis();
}

String classifyState(int piezo_left, int piezo_right, int flex_middle){
  if (flex_middle > 764 || piezo_left == piezo_right) return "middle";
  else if(piezo_left > piezo_right && piezo_right != 0) return "left";
  else if (piezo_left < piezo_right && piezo_left != 0) return "right";
  else if (piezo_left == 0) return "left";
  else if (piezo_right == 0) return "right";
}

void loop() {
  int sensorValue1 = analogRead(piezoPin);  // read sensor 1 LEFT
  //scaling factor applied to the rightside sensor as it is on average more sensitive, for meaningful data to be gained 
  //both sensors should have similar baselines
  int sensorValue2 = analogRead(piezoPin2) / 3.5; // read sensor 2 RIGHT
  int flexValue = analogRead(flexPin); // read flex sensor MIDDLE 
  
  if (millis() - startTime < duration){
    unsigned long timestamp = millis() - startTime;

    String label = classifyState(sensorValue1, sensorValue2, flexValue);
  
    Serial.println("Time");
    Serial.print(timestamp);
    Serial.print(" ms, Piezo Value Left: ");
    Serial.println(sensorValue1);
    Serial.print("Piezo Value Right: ");
    Serial.println(sensorValue2);
    Serial.print("Flex Value Middle: ");
    Serial.println(flexValue);

    file = SD.open(filename, FILE_WRITE);
    if(file){
      file.print(timestamp);
      file.print(", ");
      file.print(sensorValue1);
      file.print(", ");
      file.print(sensorValue2);
      file.print(", ");
      file.print(flexValue);
      file.print(", ");
      file.println(label);
      file.close();
    } else {
      Serial.println("Error opening file");
    }

    delay(10);
  } else {
    Serial.println("Data Collection Complete");
    while(true);
  }
}

