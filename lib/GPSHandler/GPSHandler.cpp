#include <GPSHandler.h>


GPSHandler::GPSHandler(HardwareSerial *serial) {
  gpsSerial.begin(serial, 9600);
}

int GPSHandler::update() {
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
