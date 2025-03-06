#include <SPI.h>
#include <SD.h>

#define CS_PIN 10 //chipset for sd

//range finder 1 (finder on left)
#define TRIG1 6
#define ECHO1 7

//range finder 2 (finder on right)
#define TRIG2 8
#define ECHO2 9

File myFile;

void setup() {
  Serial.begin(9600);

  if(!SD.begin(CS_PIN))
  {
    Serial.println("SD card failed");
    return;
  }
  Serial.println("SD card Found");

  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);

  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
}

long getDist(int trigPin, int echoPin){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long dur = pulseIn(echoPin, HIGH);
  return (dur / 2) * 0.0344; //converts to cm rather than m
}

String classifyState(int p1, int p2, int p3, int p4, int p5, long d1, long d2) {
  int cumPress = p1 + p2 + p3 + p4 + p5;
  long avgDist = (d1 + d2) / 2;

  if (cumPress > 900 && avgDist < 20 && avgDist > 40) return "lying"; //VALUES SUBJECT TO CHANGE
  if (cumPress > 900 && d1 < d2) return "lying_toward_left"; //VALUES SUBJECT TO CHANGE
  if (cumPress > 900 && d1 > d2) return "lying_toward_right"; //VALUES SUBJECT TO CHANGE
  if (cumPress < 900) return "empty"; //VALUES SUBJECT TO CHANGE
  return "unkown"; //default
}

void loop() {
  int pressure_left = analogRead(A0);
  int pressure_right = analogRead(A1);
  int pressure_centre = analogRead(A2);
  int pressure_top = analogRead(A3);
  int pressure_bottom = analogRead(A4);

  long dist_left = getDist(TRIG1, ECHO1);
  delay(50);
  long dist_right = getDist(TRIG2, ECHO2);

  unsigned long timestamp = millis();
  //gives label for what is happening (pre-processing)
  String label = classifyState(pressure_left, pressure_right, pressure_centre, pressure_top, pressure_bottom, dist_left, dist_right);

  myFile = SD.open("data.csv", FILE_WRITE);

  if (myFile) {
    myFile.print(timestamp);
    myFile.print(",");
    myFile.print(pressure_left);
    myFile.print(",");
    myFile.print(pressure_right);
    myFile.print(",");
    myFile.print(pressure_centre);
    myFile.print(",");
    myFile.print(pressure_top);
    myFile.print(",");
    myFile.print(pressure_bottom);
    myFile.print(",");
    myFile.print(dist_left);
    myFile.print(",");
    myFile.print(dist_right);
    myFile.print(",");
    myFile.println(label);
    myFile.close();
    Serial.println("Data saved!");
  } else {
    Serial.println("Error opening file.");
  }
}