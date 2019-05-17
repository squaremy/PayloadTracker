// #include <Arduino.h>
// #include <SD.h>
// #include <SPI.h>
// #include <Adafruit_BMP280.h>
// #include <GPSHandler.h>
// #include <RF24.h>
//
// /**
//  * Program: Rocket Payload
//  * Author: Jordan Martin
//  * Version: 1.1
//  * Date Created: 5 March 2019
//  * Description: Tester for rocket payloads. Also program to develop payload code.
//  * Last Edited by: Jordan Martin
//  * Last Edited: 8 May 2019
//  * Reason edited: Updating data collection (adding frequencies)
//  */
// #define SD_CSPIN PB5
// #define BUZZER_PIN PA1
// #define ALTITUDE_FREQUENCY 16
// #define ACCELERATION_FREQUENCY 8
// #define ORIENTATION_FREQUENCY 8
//
// /*
// SPI1
// SS -> PA4
// SCK -> PA5
// MISO -> PA6
// MOSI -> PA7
//
// SPI2
// SS -> PB12
// SCK -> PB13
// MISO -> PB14
// MOSI -> PB15
//
// */
//
// // GPSHandler gps(&Serial1);
// GPSHandler::GPSData gpsDat;
// RF24 radio(PB6, PB13); // CE, CSN
// const byte address[6] = "00001";
// Adafruit_BMP280 pressureAltitude;
// float zeroAltitudePressure = 0;
// File flightData;
// String filename;
//
// String getFilename(String baseName) {
//   int curFile = 0;
//   char toCheck[100] = "";
//   while(true) {
//     sprintf(toCheck, "%s%d.txt", baseName.c_str(), curFile);
//     if(!SD.exists(toCheck)) return toCheck;
//     curFile++;
//   }
// }
//
// String checksum(float x, float y) { // general algorithm
//   char toReturn[100] = ""; // string to return
//   double temp = x + y; // combine x and y
//   int sum = 0; // init sum var
//   for(char i : String(temp)){ // go through each char in the string of temp
//     sum += i; // add ascii values to sum
//   }
//   sum %= 0xff; // limit to 2 hexadecimal characters
//   sprintf(toReturn, "*%X", sum); // convert to hexadecimal value with * in front (checksum)
//   return toReturn; // return checksum
// }
//
// String floatToString(float val, int length) {
//   String toReturn = String(val, length);
//   return toReturn;
// }
//
// void disableSPI() {
//   digitalWrite(PB6, LOW);
//   digitalWrite(PB7, HIGH);
//   digitalWrite(SD_CSPIN, HIGH);
// }
//
// enum Device {
//   RADIO, SD_READER
// };
// void enableSPIDevice(Device toEnable) { // TODO: test
//   disableSPI();
//   switch (toEnable) {
//     case RADIO:
//       radio.begin(); // start the radio transmitter
//       radio.openWritingPipe(address); // start radio on certain address
//       radio.setPALevel(RF24_PA_MIN); // sets power amplifier level
//       radio.stopListening(); // stop looking for messages (this radio isn't intended to receive messages yet)
//       break;
//     case SD_READER:
//       digitalWrite(SD_CSPIN, LOW);
//       break;
//     default:
//       radio.begin(); // start the radio transmitter
//       radio.openWritingPipe(address); // start radio on certain address
//       radio.setPALevel(RF24_PA_MIN); // sets power amplifier level
//       radio.stopListening(); // stop looking for messages (this radio isn't intended to receive messages yet)
//       break;
//   }
// }
//
// SPIClass SPI_TWO(2);
//
// void setup()
// {
//   pinMode(PC13, OUTPUT);
//   // SPI.begin();
//   SPI_TWO.begin();
//   SPI_TWO.setClockDivider(SPI_CLOCK_DIV16);
//   pinMode(BUZZER_PIN, OUTPUT);
//   digitalWrite(PC13, HIGH);
//   delay(250);
//   digitalWrite(PC13, LOW);
//   delay(250);
//   Serial2.begin(115200);
//   disableSPI();
//   // pressureAltitude.begin();
//   // zeroAltitudePressure = pressureAltitude.readPressure()/100.0;
//   Serial2.println("GPS Start"); //Just show to the monitor that the sketch has started
//   enableSPIDevice(RADIO);
// }
//
// int i = 1;
// int lastPos = 0;
// long startTimeAltitude = millis();
// long startTimeAcceleration = startTimeAltitude;
// long startTimeOrientation = startTimeAltitude;
// void loop()
// {
//   char txt[100] = "Test Line";
//   // sprintf(txt, "Test%f", pressureAltitude.readAltitude(zeroAltitudePressure));
//   radio.write(&txt, sizeof(txt));
//   digitalWrite(BUZZER_PIN, HIGH);
//   digitalWrite(PC13, HIGH);
//   delay(1000);
//   digitalWrite(BUZZER_PIN, LOW);
//   digitalWrite(PC13, LOW);
//   delay(500);
// }
