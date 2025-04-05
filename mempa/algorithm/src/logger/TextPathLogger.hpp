#pragma once

#include "PathLogger.hpp"
/**
 * @brief Outputs path data in text format (original format)
 */
class TextPathLogger : public PathLogger {
public:
  void logPath(const std::string &filename,
               const std::vector<std::pair<int, int>> &path);
};
