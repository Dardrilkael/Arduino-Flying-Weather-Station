#pragma once
#include "station_config.h"
#include <string>
#include <vector>
class SdManager {
public:
    bool begin();
    bool createDirectory(const char* directory);
    bool appendToFile(const char* path, const char* message);
    bool storeMeasurement(const char* directory, const char* fileName, const char* payload, const char* ext = "txt");
    bool loadConfiguration(const char* path, Config &config, std::string& configJson);
    //bool csvToJsonLines(const char* path, std::vector<std::string>& jsonLines, size_t maxLines = 5);
};
