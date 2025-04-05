#pragma once

#include "JsonPathLogger.h"
#include "PathLogger.h"
#include <string>

/**
 * @brief Outputs path data in JSON format with separate files for analytics and
 * path
 */
class JsonPathLogger : public PathLogger {
public:
  void logPath(const std::string &filename,
               const std::vector<std::pair<int, int>> &path);
};
