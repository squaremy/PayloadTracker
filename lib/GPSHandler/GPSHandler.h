#ifndef _GPSHANDLER_H_
#define _GPSHANDLER_H_
#include <Arduino.h>
#include <BetterSerial.h>

/**
 * Library: GPSHandler
 * Author: Jordan Martin
 * Version: 1.0
 * Date Created: 9 Mar 2019
 * Description: A custom library to help in parsing raw GPS data
 * Last Edited By: Jordan Martin
 * Date Edited: 25 Mar 2019
 * Reason Edited: Fixed DMtoDecimal (wasn't of type float)
 */
#define GPS_FIXED 1
#define GPS_NOT_FIXED 0
#define NO_NEW_DATA -1

class GPSHandler { // class overview
public:
  struct GPSData{ // contains all GPS data we need
    float latitude;
    float longitude;
    float altitude;
    long timestamp;
    int satellites;
    int fixQuality;
    long timeSinceLastUpdate;
  };
  GPSHandler(HardwareSerial *serial); // constructor
  int update(); // updates gps data if available
  int getGPSData(GPSData *gpsData); // obtains and parses gps data
  int getRawData(String *data); // gets raw gps data
private:
  int parseData(String toParse, GPSData *parsedData); // parses gps data
  float DMtoDecimal(String dm); // converts gps lat and lng from DDMM.MMMM (degMin.Min) to degrees
  bool dataGood = false;
  long timeAtLastUpdate = 0;
  BetterSerial gpsSerial;
  GPSData curGPSData;
  String rawData;
};

#endif
