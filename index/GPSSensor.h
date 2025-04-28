#pragma once
#include <TinyGPS++.h>
#include <HardwareSerial.h>

class GPSSensor {
public:
    GPSSensor(HardwareSerial& serial, int rxPin, int txPin, uint32_t baud = 9600);
    void begin();
    void update();
    bool isLocationValid();
    double getLatitude();
    double getLongitude();
    String getDateTime();
    time_t getUnixTime();
    bool isTimeValid();

private:
    TinyGPSPlus gps;
    HardwareSerial& hwSerial;
    int _rx, _tx;
    uint32_t _baud;
};
