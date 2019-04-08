#include <Arduino.h>
// #include <TinyGPS++.h>
// #include <SoftwareSerial.h>
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
 * Reason edited: Fixing SPI devices
 */
GPSHandler gps(&Serial);
GPSHandler::GPSData gpsDat;
// SoftwareSerial serial_connection(3, 2); //RX=pin 3, TX=pin 2 (for arduino... connect TX to RX and vice-versa)
// TinyGPSPlus gps;//This is the GPS object that will pretty much do all the grunt work with the NMEA data
RF24 radio(9, 10); // CE, CSN
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

// String checksum(TinyGPSLocation loc) {
//   return (checksum(loc.lat(), loc.lng()));
// }
//
// String checksum(TinyGPSTime time, float alt) {
//   String hr = String(time.hour());
//   if(time.hour() < 10) hr = "0" + hr;
//   String min = String(time.minute());
//   if(time.minute() < 10) min = "0" + min;
//   String sec = String(time.second());
//   if(time.second() < 10) sec = "0" + sec;
//   float newTime = atof((hr + min + sec).c_str());
//   return (checksum(newTime, alt));
// }

String floatToString(float val, int length) {
  String toReturn = String(val, length);
  return toReturn;
}

void disableSPI() {
  digitalWrite(9, LOW);
  digitalWrite(10, HIGH);
  digitalWrite(6, HIGH);
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
      digitalWrite(6, LOW);
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
  Serial.begin(9600);//This opens up communications to the Serial monitor and GPS
  // serial_connection.begin(9600);//This opens up communications to the GPS
  enableSPIDevice(SD_READER);
  if(!SD.begin(6)) {
    Serial.println("SD init failed");
  }
  flightData = SD.open("fdat0.txt", FILE_WRITE);
  flightData.println("Loops:");
  flightData.close();
  disableSPI();
  zeroAltitudePressure = pressureAltitude.readPressure();
  Serial.println("GPS Start");//Just show to the monitor that the sketch has started
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
    // Serial.println(location);

    // char altitude[100] = "";
    // sprintf(altitude, "PA:%sTS:%s%s", floatToString(gpsDat.altitude, sizeof(gpsDat.altitude)/sizeof(float)).c_str(), String(gpsDat.timestamp).c_str(), checksum(gpsDat.altitude, gpsDat.timestamp).c_str());
    // radio.write(&altitude, sizeof(altitude));
    // Serial.println(altitude);
  }
  enableSPIDevice(RADIO);
  Serial.print("PA sensor: ");
  float alt = pressureAltitude.readAltitude(zeroAltitudePressure);
  Serial.print(alt);
  Serial.println();
  char altitude[100] = "";
  sprintf(altitude, "PA:%s", floatToString(alt, 6).c_str());
  radio.write(&altitude, sizeof(altitude));
  Serial.println(altitude);
  Serial.println();

  enableSPIDevice(SD_READER);
  flightData = SD.open("fdat0.txt", FILE_WRITE);
  flightData.seek(lastPos);
  flightData.println(i);
  lastPos = flightData.position();
  flightData.close();
  i++;
  delay(1000);
  // while(serial_connection.available())//While there are characters to come from the GPS
  // {
  //   gps.encode(serial_connection.read());//This feeds the serial NMEA data into the library one char at a time
  // }
  // if(gps.location.isUpdated())//This will pretty much be fired all the time anyway but will at least reduce it to only after a package of NMEA data comes in
  // {
  //   // compile and send location data
  //   char location[100] = ""; // create location character array
  //   sprintf(location, "LX:%sLY:%s%s", floatToString(gps.location.lat(), 6).c_str(), floatToString(gps.location.lng(), 6).c_str(), checksum(gps.location).c_str());
  //   radio.write(&location, sizeof(location)); // send location data to receiver through the radio
  //   Serial.println(location);
  //
  //   // compile and send altitude data
  //   char altitude[100] = ""; // create altitude charachter array
  //   String hr = String(gps.time.hour());
  //   if(gps.time.hour() < 10) hr = "0" + hr;
  //   String min = String(gps.time.minute());
  //   if(gps.time.minute() < 10) min = "0" + min;
  //   String sec = String(gps.time.second());
  //   if(gps.time.second() < 10) sec = "0" + sec;
  //   String compiledTime = hr + min + sec;
  //   sprintf(altitude, "PA:%sTS:%s%s", floatToString(gps.altitude.meters(), sizeof(gps.altitude.meters())/sizeof(double)).c_str(), compiledTime.c_str(), checksum(gps.time, gps.altitude.meters()).c_str());
  //   radio.write(&altitude, sizeof(altitude)); // send altitude data to receiver through the radio
  //   Serial.println(altitude);
  // }
}
