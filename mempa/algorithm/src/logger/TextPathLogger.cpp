#include "TextPathLogger.hpp"
#include <fstream>
#include <string>

void TextPathLogger::logPath(const std::string &filename,
                             const std::vector<std::pair<int, int>> &path) {

  // Original text output implementation
  std::ofstream outFile(filename);
  if (!outFile) {
    throw std::runtime_error("Failed to create output file: " + filename);
  }

  // outFile << std::fixed << std::setprecision(3);
  // outFile << "# Mars Rover Path Analysis\n";
  // outFile << "# Generated: " << std::time(nullptr) << "\n";
  // outFile << "# Start: (" << startPoint.first << "," << startPoint.second
  //         << ")\n";
  // outFile << "# End: (" << endPoint.first << "," << endPoint.second << ")\n";
  // outFile << "# Metrics:\n";
  // outFile << "#   Total Distance: " << metrics.totalDistance << " m\n";
  // outFile << "#   Horizontal Distance: " << metrics.horizontalDistance
  //         << " m\n";
  // outFile << "#   Total Elevation Change: " << metrics.totalElevationChange
  //         << " m\n";
  // outFile << "#   Net Elevation Change: " << metrics.netElevationChange
  //         << " m\n";
  // outFile << "#   Maximum Slope: " << metrics.maxSlope << "°\n";
  // outFile << "#   Average Slope: " << metrics.averageSlope << "°\n";
  // outFile << "#   Energy Cost: " << metrics.energyCost << "\n";
  // outFile << "#   Base Elevation: " << metrics.baseElevation << " m\n";
  // outFile << "#   Waypoints: " << metrics.waypointCount << "\n\n";
  outFile << "# X Y \n";

  for (auto path_coord : path) {
    outFile << path_coord.first << " " << path_coord.second << "\n";
  }

  outFile.close();
}
