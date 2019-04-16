#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <GPSHandler.h>
#include <RF24.h>

/**
 * Program: Rocket Payload
 * Author: Jordan Martin
 * Version: 1.0
 * Date Created: 5 March 2019
 * Description: Tester for rocket payloads. Also program to develop payload code.
 * Last Edited by: Jordan Martin
 * Last Edited: 2 April 2019
 * Reason edited: Cleanup and updated for stm32f103c8t6 (blue pill)
 */
#define SD_CSPIN PB5

GPSHandler gps(&Serial1);
GPSHandler::GPSData gpsDat;
RF24 radio(PB6, PB7); // CE, CSN
const byte address[6] = "00001";
Adafruit_BMP280 pressureAltitude;
float zeroAltitudePressure = 0;
File flightData;

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

String floatToString(float val, int length) {
  String toReturn = String(val, length);
  return toReturn;
}

void disableSPI() {
  digitalWrite(PB6, LOW);
  digitalWrite(PB7, HIGH);
  digitalWrite(SD_CSPIN, HIGH);
}

enum Device {
  RADIO, SD_READER
};
void enableSPIDevice(Device toEnable) { // TODO: test
  disableSPI();
  switch (toEnable) {
    case RADIO:
      radio.begin(); // start the radio transmitter
      radio.openWritingPipe(address); // start radio on certain address
      radio.setPALevel(RF24_PA_MIN); // sets power amplifier level
      radio.stopListening(); // stop looking for messages (this radio isn't intended to receive messages yet)
      break;
    case SD_READER:
      digitalWrite(SD_CSPIN, LOW);
      break;
    default:
      radio.begin(); // start the radio transmitter
      radio.openWritingPipe(address); // start radio on certain address
      radio.setPALevel(RF24_PA_MIN); // sets power amplifier level
      radio.stopListening(); // stop looking for messages (this radio isn't intended to receive messages yet)
      break;
  }
}

void setup()
{
  Serial2.begin(115200);
  Serial1.begin(9600); //This opens up communications to the Serial monitor and GPS
  enableSPIDevice(SD_READER);
  if(!SD.begin(SD_CSPIN)) {
    Serial.println("SD init failed");
  }
  flightData = SD.open("fdat0.txt", FILE_WRITE);
  flightData.println("Loops:");
  flightData.close();
  pressureAltitude.begin();
  disableSPI();
  zeroAltitudePressure = pressureAltitude.readPressure();
  Serial2.println("GPS Start"); //Just show to the monitor that the sketch has started
  enableSPIDevice(SD_READER);
}

int i = 1;
int lastPos = 0;
void loop()
{
  if(gps.update() == 0) {
    gps.getGPSData(&gpsDat);

    // char location[100] = "";
    // sprintf(location, "LX:%sLY:%s%s", floatToString(gpsDat.latitude, 6).c_str(), floatToString(gpsDat.longitude, 6).c_str(), checksum(gpsDat.latitude, gpsDat.longitude).c_str());
    // radio.write(&location, sizeof(location));
    // Serial2.println(location);

    // char altitude[100] = "";
    // sprintf(altitude, "PA:%sTS:%s%s", floatToString(gpsDat.altitude, sizeof(gpsDat.altitude)/sizeof(float)).c_str(), String(gpsDat.timestamp).c_str(), checksum(gpsDat.altitude, gpsDat.timestamp).c_str());
    // radio.write(&altitude, sizeof(altitude));
    // Serial2.println(altitude);
  }
  enableSPIDevice(RADIO);
  Serial2.print("PA sensor: ");
  float alt = pressureAltitude.readAltitude(zeroAltitudePressure);
  Serial2.print(alt);
  Serial2.println();
  char altitude[100] = "";
  sprintf(altitude, "PA:%s", floatToString(alt, 6).c_str());
  radio.write(&altitude, sizeof(altitude));
  Serial2.println(altitude);
  Serial2.println();

  enableSPIDevice(SD_READER);
  flightData = SD.open("fdat0.txt", FILE_WRITE);
  flightData.seek(lastPos);
  flightData.println(i);
  lastPos = flightData.position();
  flightData.close();
  i++;
  delay(1000);
}
