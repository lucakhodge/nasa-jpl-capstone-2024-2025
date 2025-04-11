#include "JsonPathLogger.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <vector>

void JsonPathLogger::logPath(const std::string &filename,
                           const std::vector<std::pair<int, int>> &path,
                           Metrics metrics) {
  // print outPath in JSON format to file for GUI to read
  nlohmann::json j;
  j["data"] = nlohmann::json::array();

  // convert pair<int, int> in outPath to JSON format
  for (const auto &point : path) {
    j["data"].push_back({{"x", point.first}, {"y", point.second}});
  }

  // CHANGE HERE: Make metrics a direct object instead of an array element
  j["metrics"] = {
    {"horizontalDistance", metrics.horizontalDistance},
    {"asTheCrowFlysDistance", metrics.asTheCrowFlysDistance},
    {"totalDistance", metrics.totalDistance},
    {"totalElevationChange", metrics.totalElevationChange},
    {"netElevationChange", metrics.netElevationChange},
    {"maxSlope", metrics.maxSlope},
    {"averageSlope", metrics.averageSlope}
  };

  // Add debug output to verify metrics are being serialized correctly
  std::cout << "\n===== METRICS VALUES BEING WRITTEN TO JSON =====\n";
  std::cout << "horizontalDistance: " << metrics.horizontalDistance << std::endl;
  std::cout << "asTheCrowFlysDistance: " << metrics.asTheCrowFlysDistance << std::endl;
  std::cout << "totalDistance: " << metrics.totalDistance << std::endl;
  std::cout << "totalElevationChange: " << metrics.totalElevationChange << std::endl;
  std::cout << "netElevationChange: " << metrics.netElevationChange << std::endl;
  std::cout << "maxSlope: " << metrics.maxSlope << std::endl;
  std::cout << "averageSlope: " << metrics.averageSlope << std::endl;

  std::ofstream jsonFile(filename);
  if (jsonFile.is_open()) {
    jsonFile << j.dump(2); // Added indentation for better readability
    jsonFile.close();
    std::cout << "Path saved to " << filename << std::endl;
  } else {
    std::cerr << "Error: Could not open file to write JSON." << std::endl;
  }
}