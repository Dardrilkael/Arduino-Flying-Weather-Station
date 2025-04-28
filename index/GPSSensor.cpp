#include "GPSSensor.h"
#include <Arduino.h>
time_t gpsToUnixTime(int year, int month, int day, int hour, int minute, int second);
GPSSensor::GPSSensor(HardwareSerial& serial, int rxPin, int txPin, uint32_t baud)
    : hwSerial(serial), _rx(rxPin), _tx(txPin), _baud(baud) {}

void GPSSensor::begin() {
    hwSerial.begin(_baud, SERIAL_8N1, _rx, _tx);
}

void GPSSensor::update() {
    while (hwSerial.available()) {
        gps.encode(hwSerial.read());
    }
}

bool GPSSensor::isLocationValid() {
    return gps.location.isValid();
}

double GPSSensor::getLatitude() {
    return gps.location.lat();
}

double GPSSensor::getLongitude() {
    return gps.location.lng();
}

String GPSSensor::getDateTime() {
    if (gps.date.isValid() && gps.time.isValid()) {
        char buffer[25]{0};
        sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d",
            gps.date.day(), gps.date.month(), gps.date.year(),
            gps.time.hour(), gps.time.minute(), gps.time.second());
        return String(buffer);
    }
    return "Invalid";
}

    time_t GPSSensor::getUnixTime(){
      if (gps.date.isValid() && gps.time.isValid())
        return gpsToUnixTime(gps.date.year(),gps.date.month(), gps.date.day(),gps.time.hour(),gps.time.minute(),gps.time.second());
      return gpsToUnixTime(0,0,0,0,0,0);
    }

    bool GPSSensor::isTimeValid(){
      if (gps.date.isValid() && gps.time.isValid()){
        return (getUnixTime()>1745617523);
      }
      return false;
    }


time_t gpsToUnixTime(int year, int month, int day, int hour, int minute, int second) {
    struct tm t = {};
    t.tm_year = year - 1900;
    t.tm_mon  = month - 1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min  = minute;
    t.tm_sec  = second;

    time_t gpsTime = mktime(&t);
    return gpsTime;
}
