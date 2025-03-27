#include <NewPing.h>
#include <SPI.h>
#include <SD.h>

#define trigPin_left 6
#define echoPin_left 7

#define trigPin_right 8
#define echoPin_right 9

#define max_distance 300
#define threshold 10

NewPing rf_right(trigPin_right, echoPin_right, max_distance);
NewPing rf_left(trigPin_left, echoPin_left, max_distance);

const unsigned long duration = 90000;
unsigned long startTime;

unsigned long lastPingTimeLeft = 0;
unsigned long lastPingTimeRight = 0;
const unsigned long pingInterval = 100; 

float lastRightVal = 0;
float lastLeftVal = 0;

int chipSelect = 10; //chip select pin for the MicroSD Card Adapter
File file; // file object that is used to read and write data
String filename = "RBT3.csv";

void setup() {
  Serial.begin(9600);
  pinMode(trigPin_left, OUTPUT);
  pinMode(echoPin_left, INPUT);

  pinMode(trigPin_right, OUTPUT);
  pinMode(echoPin_right, INPUT);

  pinMode(chipSelect, OUTPUT);

  if(!SD.begin(chipSelect)){
    Serial.println("SD card not found");
    return;
  }
  Serial.println("SD card found!");

  file = SD.open(filename, FILE_WRITE);
  if(file){
    file.println("Timestamp (ms), rf Distance left, rf Distance right, Label");
    file.close();
  } else{ 
    Serial.println("Failed to open file"); 
    }

  startTime = millis();
}

String classifyState(float dist_left, float dist_right){
  float diff = abs(dist_left - dist_right);
  if (diff <= threshold && dist_left != 0 && dist_right != 0) return "middle";
  else if(dist_left < dist_right || (dist_right == 0 && dist_left != 0)) return "left";
  else if(dist_right < dist_left || (dist_right != 0 && dist_left == 0)) return "right";
  else return "N/A";
}

void loop() {
  unsigned long current = millis();

  if (millis() - startTime < duration){
    unsigned long timestamp = millis() - startTime;

    float dist_right = 0;
    float dist_left = 0;

    // Trigger left sensor if enough time has passed
    if (current - lastPingTimeLeft >= pingInterval) {
      lastPingTimeLeft = current;
      dist_left = rf_left.ping_cm();
    }

    // Trigger right sensor if enough time has passed
    if (current - lastPingTimeRight >= pingInterval) {
      lastPingTimeRight = current;
      dist_right = rf_right.ping_cm();
    }

    if(dist_left != 0)
    {
      lastLeftVal = dist_left;
    }
    else if (dist_left == 0){
      dist_left = lastLeftVal;
    }

    if(dist_right != 0)
    {
      lastRightVal = dist_right;
    }
    else if (dist_right == 0){
      dist_right = lastRightVal;
    }

    String label = classifyState(dist_left, dist_right);

    Serial.println("Time");
    Serial.println(timestamp);
    Serial.println("Distance Left");
    Serial.println(dist_left);
    Serial.println(" cm");
    Serial.println("Distance Right");
    Serial.println(dist_right);
    Serial.println(" cm");
    Serial.print("Label: ");
    //Serial.print(label);

    file = SD.open(filename, FILE_WRITE);
    if(file){
      file.print(timestamp);
      file.print(", ");
      file.print(dist_left);
      file.print(", ");
      file.print(dist_right);
      file.print(", ");
      file.println(label);
      file.close();
    } else {
      Serial.println("Error opening file");
    }
  } else {
    Serial.println("Data Collected");
    while(true);
  }
}
