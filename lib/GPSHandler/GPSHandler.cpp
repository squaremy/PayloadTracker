#include <GPSHandler.h>

/**
 * Library: GPSHandler
 * Author: Jordan Martin
 * Version: 1.0
 * Date Created: 9 Mar 2019
 * Description: A custom library to help in parsing raw GPS data
 * Last Edited By: Jordan Martin
 * Date Edited: 11 Mar 2019
 * Reason Edited: Added getRawData()
 */
GPSHandler::GPSHandler(HardwareSerial *serial) { // set up gps communication
  gpsSerial.begin(serial, 9600);
}

int GPSHandler::update() { // update gps data if available
  int updates = 0;
  GPSData tempData;
  while(gpsSerial.readLine(&rawData)) {
    updates++;
    if(parseData(rawData, &tempData) == 0) {
      if(!dataGood) dataGood = true;
      tempData.timeSinceLastUpdate = timeAtLastUpdate - millis();
      timeAtLastUpdate = millis();
      curGPSData = tempData;
      updates++;
    }
  }
  if(updates > 0) return 0;
  if(!dataGood) return GPS_NOT_FIXED;
  return NO_NEW_DATA;
}

int GPSHandler::getGPSData(GPSData *gpsData) { // obtain current gps data
  if(!dataGood) return -1;
  *gpsData = curGPSData;
  return 0;
}

int GPSHandler::getRawData(String *data) {
  return rawData;
}

int GPSHandler::parseData(String toParse, GPSData *parsedData) { // parse raw data
  if(toParse.indexOf("$GPGGA") == -1) return -1;
  int commaIndicies[13]; // keep track of the data divisions (where the commas are)
  commaIndicies[0] = toParse.indexOf(",");
  for(int i = 1; i < 13; i++) {
    commaIndicies[i] = toParse.indexOf(",", commaIndicies[i-1]+1);
    if(commaIndicies[i] < 0 || commaIndicies[i] >= (int)toParse.length()) return -1;
  }
  int latDir, lngDir;
  if(strcmp(toParse.substring(commaIndicies[2]+1, commaIndicies[3]).c_str(), "N")) latDir = 1;
  else latDir = -1;
  if(strcmp(toParse.substring(commaIndicies[4]+1, commaIndicies[5]).c_str(), "E")) lngDir = 1;
  else lngDir = -1;
  parsedData->timestamp = atol(toParse.substring(commaIndicies[0]+1, commaIndicies[1]).c_str());
  parsedData->altitude = atof(toParse.substring(commaIndicies[8]+1, commaIndicies[9]).c_str());
  parsedData->latitude = DMtoDecimal(atof(toParse.substring(commaIndicies[1]+1, commaIndicies[2]).c_str())) * latDir;
  parsedData->longitude = DMtoDecimal(atof(toParse.substring(commaIndicies[3]+1, commaIndicies[4]).c_str())) * lngDir;
  parsedData->satellites = atoi(toParse.substring(commaIndicies[6]+1, commaIndicies[7]).c_str());
  switch(atoi(toParse.substring(commaIndicies[5]+1, commaIndicies[6]).c_str())) {
    case 1:
      parsedData->fixQuality = GPS_FIXED;
      break;
    default:
      parsedData->fixQuality = GPS_NOT_FIXED;
      break;
  }
  return 0;
}

int GPSHandler::DMtoDecimal(float dm) { // convert gps lat and lng data to degrees
  float deg = atof(String(dm).substring(0, 2).c_str());
  float min = atof(String(dm).substring(2, String(dm).length()).c_str());
  return (deg + (min/60.0));
}
