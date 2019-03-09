#ifndef _GPSHANDLER_H_
#define _GPSHANDLER_H_
#include <Arduino.h>
#include <BetterSerial.h>

#define GPS_FIXED 1
#define GPS_NOT_FIXED 0
#define NO_NEW_DATA -1

class GPSHandler {
public:
  struct GPSData{
    float latitude;
    float longitude;
    float altitude;
    long timestamp;
    int satellites;
    int fixQuality;
    long timeSinceLastUpdate;
  };
  GPSHandler(HardwareSerial *serial);
  int update();
  int getGPSData(GPSData *gpsData);
private:
  int parseData(String toParse, GPSData *parsedData);
  int DMtoDecimal(float dm);
  bool dataGood = false;
  long timeAtLastUpdate = 0;
  BetterSerial gpsSerial;
  GPSData curGPSData;
  String rawData;
};

#endif
