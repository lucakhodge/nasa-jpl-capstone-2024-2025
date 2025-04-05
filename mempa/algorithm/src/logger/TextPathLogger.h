#pragma once

#include "PathLogger.h"
/**
 * @brief Outputs path data in text format (original format)
 */
class TextPathLogger : public PathLogger {
public:
  void logPath(const std::string &filename,
               const std::vector<std::pair<int, int>> &path,
               std::pair<int, int> startPoint, std::pair<int, int> endPoint,
               const TerrainMetrics &metrics);
};
