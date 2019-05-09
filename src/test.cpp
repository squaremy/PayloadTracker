#include <Arduino.h>
#include <Servo.h>
#include <SPI.h>
#include <SD.h>
#include <RF24.h>

/**
 * Program: Payload Tester
 * Author: Jordan Martin
 * Version: 1.0
 * Date Created: 8 May 2019
 * Description: Program to test payload operations, especially over radio
 * Last Edited by: Jordan Martin
 * Last Edited: 8 May 2019
 * Reason edited: Initial edit
 */
 
#define SERVO PB3
#define RADIO_CE PB6
#define RADIO_CSN PB7
#define BUZZER PB4
#define CAMERA PB8
#define SD_CSPIN PB5

Servo parachute, camera;
RF24 radio(RADIO_CE, RADIO_CSN);
const byte addresses[][6] = {"00001", "00002"};
int dataPoints[100][2];
File testFile;

String checksum(float x, float y) { // general algorithm
  char toReturn[100] = ""; // string to return
  double temp = x + y; // combine x and y
  int sum = 0; // init sum var
  for(char i : String(temp)){ // go through each char in the string of temp
    sum += i; // add ascii values to sum
  }
  sum %= 0xff; // limit to 2 hexadecimal characters
  sprintf(toReturn, "*%X", sum); // convert to hexadecimal value with * in front (checksum)
  return toReturn; // return checksum
}

void setup() {
  pinMode(BUZZER, OUTPUT);
  parachute.attach(SERVO);
  camera.attach(CAMERA);
  if(!SD.begin(SD_CSPIN)) {
    Serial.println("SD init failed");
  }
  testFile = SD.open("testfile.txt", FILE_WRITE);
  testFile.println("Initial Line");
  testFile.close();
  radio.begin();
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(0, addresses[0]);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  for(int i = 0; i < sizeof(dataPoints)/sizeof(int[2]); i++) {
    for(int j = 0; j < sizeof(dataPoints[i])/sizeof(int); j++) {
      if(j == 0) dataPoints[i][j] = i;
      else dataPoints[i][j] = -(1.0/25.0) * pow(i-50, 2) + 100;
    }
  }
}

String datIn = "";
bool buzzerOn = false;
void loop() {
  radio.read(&datIn, sizeof(datIn));
  if(datIn.substring(0, datIn.indexOf("/r")).equalsIgnoreCase("ABORT")) {
    radio.stopListening();
    char abort[] = "ABORTED";
    radio.write(&abort, sizeof(abort));
  } else if(datIn.substring(0, datIn.indexOf("/r")).equalsIgnoreCase("SYNC")) {
    Serial.println("SYNCED");
  } else if(datIn.substring(0, datIn.indexOf("/r")).equalsIgnoreCase("ARM")) {
    Serial.println("ARMED");
  } else if(datIn.substring(0, datIn.indexOf("/r")).equalsIgnoreCase("LAUNCH")) {
    Serial.println("STARTING_COUNTDOWN");
  } else if(datIn.substring(0, datIn.indexOf("/r")).equalsIgnoreCase("WRITE_SD")) {
    radio.stopListening();
    testFile = SD.open("testfile.txt", FILE_WRITE);
    testFile.println("WRITING SD");
    testFile.close();
    char writeSD[] = "SD_WRITTEN";
    radio.write(&writeSD, sizeof(writeSD));
  } else if(datIn.substring(0, datIn.indexOf("/r")).equalsIgnoreCase("TOGGLE_BUZZER")) {
    buzzerOn = !buzzerOn;
    digitalWrite(BUZZER, buzzerOn);
    radio.stopListening();
    char toggleBuzzer[] = "TOGGLED_BUZZER";
    radio.write(&toggleBuzzer, sizeof(toggleBuzzer));
  } else if(datIn.substring(0, datIn.indexOf("/r")).equalsIgnoreCase("TOGGLE_CAMERA")) {
    camera.write(180);
    delay(500);
    camera.write(0);
    radio.stopListening();
    char toggleCamera[] = "TOGGLED_CAMERA";
    radio.write(&toggleCamera, sizeof(toggleCamera));
  } else if(datIn.substring(0, datIn.indexOf("/r")).equalsIgnoreCase("DEPLOY_CHUTE")) {
    parachute.write(180);
    delay(500);
    parachute.write(0);
    radio.stopListening();
    char deployChute[] = "DEPLOYING_CHUTE";
    radio.write(&deployChute, sizeof(deployChute));
  } else if(datIn.substring(0, datIn.indexOf("/r")).equalsIgnoreCase("REQUESTING_GRAPH_DATA")) {
    radio.stopListening();
    for(int i = 0; i < sizeof(dataPoints)/sizeof(int[2]); i++) {
        char temp[100];
        sprintf(temp, "PA:%fTS:%f%s", dataPoints[i][0], dataPoints[i][1], checksum(dataPoints[i][0], dataPoints[i][1]));
        radio.write(&temp, sizeof(temp));
    }
    char requestGraphData[] = "GRAPH_DATA_SENT";
    radio.write(&requestGraphData, sizeof(requestGraphData));
  }
  radio.startListening();
}
