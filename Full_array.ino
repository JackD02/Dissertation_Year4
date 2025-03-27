#include <NewPing.h>
#include <SPI.h>
#include <SD.h>

// defines pins for respective rangefinders
#define trigPin_left 6
#define echoPin_left 7

#define trigPin_right 8
#define echoPin_right 9

//applies the maximum distance rangefinders can see and what the max value difference between each rangefinder is
//where it can be considered the middle of the bed
#define max_distance 300
#define threshold 10

//set up of rangefinders
NewPing rf_right(trigPin_right, echoPin_right, max_distance);
NewPing rf_left(trigPin_left, echoPin_left, max_distance);

//defines pins for pressure sensors (analog)
const int piezoPin = A0;
const int piezoPin2 = A1;
const int flexPin = A2;

//setup for duration of test
const unsigned long duration = 3600000; //1 hour = 3600000
unsigned long startTime;

//checking ping times for rangefinders in order to avoid
//issues with each rangefinder hitting eachother and disrupting their values
unsigned long lastPingTimeLeft = 0;
unsigned long lastPingTimeRight = 0;
const unsigned long pingInterval = 100; 

float lastRightVal = 0;
float lastLeftVal = 0;

//Setup for saving data to Sd card
int chipSelect = 10; //chip select pin for the MicroSD Card Adapter
File file; // file object that is used to read and write data
String filename = "FAT3.csv"; //name of file to be saved 

void setup() {
  Serial.begin(9600);
  //setting pins for left rf
  pinMode(trigPin_left, OUTPUT);
  pinMode(echoPin_left, INPUT);

  //setting pins for right rf
  pinMode(trigPin_right, OUTPUT);
  pinMode(echoPin_right, INPUT);

  //selecting cs pin for sd card
  pinMode(chipSelect, OUTPUT);

  if(!SD.begin(chipSelect)){ //checks for sd card
    Serial.println("SD card not found");
    return;
  }
  Serial.println("SD card found!");

  file = SD.open(filename, FILE_WRITE);
  if(file){ //writes in initial column names for csv file
    file.println("Timestamp (ms), Piezo Value Left, Piezo Value Right, Flex Value Middle, rf Distance left, rf Distance right, Label");
    file.close();
  } else{ 
    Serial.println("Failed to open file"); 
    }

  startTime = millis();
}

String classifyState(int piezo_left, int piezo_right, int flex_middle, float rf_left, float rf_right){
  // pre-processing to label data entry with useful label of what is happening
  // useful for machine learning classification later
  float diff = abs(rf_left - rf_right); //finds difference in distance between both rf values

  //middle of the bed (either distance values within threshold or flex sensor being pressed down on)
  //flex sensor is more reliable (keep as OR gate)
  if ((diff <= threshold && rf_left != 0 && rf_right != 0) || flex_middle > 764)
   return "middle";
  //left of bed (left distance less than right or left piezo > pressure than right one)
  else if((rf_left < rf_right || (rf_right == 0 && rf_left != 0)) || piezo_left > piezo_right && piezo_right != 0 || piezo_left == 0) return "left";
  //right of bed (right distance less than left or right piezo > pressure than left one)
  else if((rf_right < rf_left || (rf_right != 0 && rf_left == 0)) || piezo_left < piezo_right && piezo_left != 0 || piezo_right == 0) return "right";
  else return "N/A"; //default label
}

void loop() {
  unsigned long current = millis();

  int sensorValue1 = analogRead(piezoPin);  // read sensor 1 LEFT
  //scaling factor applied to the rightside sensor as it is on average more sensitive, for meaningful data to be gained 
  //both sensors should have similar baselines
  int sensorValue2 = analogRead(piezoPin2) / 3.5; // read sensor 2 RIGHT
  int flexValue = analogRead(flexPin); // read flex sensor MIDDLE 
  
  if (millis() - startTime < duration){
    unsigned long timestamp = millis() - startTime;

    //initialise distance values
    float dist_right = 0;
    float dist_left = 0;

    // Trigger left rf if enough time has passed
    if (current - lastPingTimeLeft >= pingInterval) {
      lastPingTimeLeft = current; //updates variable
      dist_left = rf_left.ping_cm(); //auto converts to cm
    }

    // Trigger right rf if enough time has passed
    if (current - lastPingTimeRight >= pingInterval) {
      lastPingTimeRight = current; //updates variable
      dist_right = rf_right.ping_cm(); //auto converts to cm
    }

    if(dist_left != 0) //if actual value given
    {
      lastLeftVal = dist_left; //update last left val variable
    }
    else dist_left = lastLeftVal; //else set current value to be same as last found val

    if(dist_right != 0) //if actual value given
    {
      lastRightVal = dist_right; //update last right val variable 
    }
    else dist_right = lastRightVal; //else set current value to be same as last found val

    //call method to classify data
    String label = classifyState(sensorValue1, sensorValue2, flexValue, dist_left, dist_right);
  
  //FOR TESTING/DEBUGGING PURPOSES
    Serial.println("Time");
    Serial.print(timestamp);
    Serial.print(" ms, Piezo Value Left: ");
    Serial.println(sensorValue1);
    Serial.print("Piezo Value Right: ");
    Serial.println(sensorValue2);
    Serial.print("Flex Value Middle: ");
    Serial.println(flexValue);
    
    Serial.print("Distance left: ");
    Serial.println(dist_left);
    Serial.print("Distance right: ");
    Serial.println(dist_right);
    Serial.print("label: ");
    Serial.print(label); 
  ///
    file = SD.open(filename, FILE_WRITE);
    if(file){
      // Saving data into SD card in correct csv format
      file.print(timestamp);
      file.print(", ");
      file.print(sensorValue1);
      file.print(", ");
      file.print(sensorValue2);
      file.print(", ");
      file.print(flexValue);
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
    delay(10);
  } else {
    Serial.println("Data Collection Complete");
    while(true);
  }
}
