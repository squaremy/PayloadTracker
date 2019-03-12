#include <Arduino.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <RF24.h>
#include <SD.h>
// #include <math.h>
// #include <GPSHandler.h>

/**
 * Program: Rocket Payload
 * Author: Jordan Martin
 * Version: 0.2
 * Date Created: 5 March 2019
 * Description: Tester for rocket payloads. Also program to develop payload code.
 * Last Edited by: Jordan Martin
 * Last Edited: 11 March 2019
 * Reason edited: Adding SD card and 10 uF capacitors...
 */
SoftwareSerial serial_connection(3, 2); //RX=pin 11, TX=pin 10 (for arduino... connect TX to RX and vice-versa)
TinyGPSPlus gps;//This is the GPS object that will pretty much do all the grunt work with the NMEA data
// File dataFile;
// GPSHandler gps(&Serial);
// GPSHandler::GPSData gpsDat;
RF24 radio(9, 10);
const byte address[6] = "00001";
void setup()
{
  Serial.begin(115200);//This opens up communications to the Serial monitor in the Arduino IDE
  serial_connection.begin(9600);//This opens up communications to the GPS
  radio.begin(); // start the radio transmitter
  radio.openWritingPipe(address); // start radio on certain address
  radio.setPALevel(RF24_PA_MIN); // sets power amplifier level
  radio.stopListening(); // stop looking for messages (this radio isn't intended to receive messages yet)
  // dataFile = SD.open("GPSDAT.txt", FILE_WRITE);
  Serial.println("GPS Start");//Just show to the monitor that the sketch has started
}

float x = 0, y = 0;
void loop()
{
  // ------------TESTING CODE-------------------
  // x += 0.2;
  // y = sin(x);
  // String blah = String(x, sizeof(x)/sizeof(float)) + "," + String(y, sizeof(y)/sizeof(float));
  // char toWrite[100] = "";
  // for(int i = 0; i < (int)blah.length(); i++) {
  //   toWrite[i] = blah.charAt(i);
  // }
  // radio.write(&toWrite, sizeof(toWrite));
  // Serial.println(toWrite);
  // -------------END TESTING CODE--------------
  while(serial_connection.available())//While there are characters to come from the GPS
  {
    gps.encode(serial_connection.read());//This feeds the serial NMEA data into the library one char at a time
  }
  if(gps.location.isUpdated())//This will pretty much be fired all the time anyway but will at least reduce it to only after a package of NMEA data comes in
  {
    // Get the latest info from the gps object which it derived from the data sent by the GPS unit
    // Serial.println("Satellite Count:");
    // Serial.println(gps.satellites.value());
    // Serial.println("Latitude:");
    // Serial.println(gps.location.lat(), 6);
    // Serial.println("Longitude:");
    // Serial.println(gps.location.lng(), 6);
    // Serial.println("Speed MPH:");
    // Serial.println(gps.speed.mph());
    // Serial.println("Altitude Meters:");
    // Serial.println(gps.altitude.meters());
    // Serial.println("");

    // compile and send location data
    // gps.update();
    // gps.getGPSData(&gpsDat);
    String temp = String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6); // compile into string to be used later
    char location[temp.length()+1] = ""; // create location character array
    for(int i = 0; i < (int)temp.length(); i++) { // go through each piece of the string data
      location[i] = temp.charAt(i); // add current piece of string to char array
    }
    radio.write(&location, sizeof(location)); // send location data to receiver through the radio
    Serial.println(location);
    // dataFile.println(gps.getRawData());
    delay(50);
  }
}
