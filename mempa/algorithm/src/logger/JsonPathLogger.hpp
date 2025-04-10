#pragma once

#include "JsonPathLogger.hpp"
#include "PathLogger.hpp"
#include <string>
#include <utility>
#include <vector>

/**
 * @brief Outputs path data in JSON format with separate files for analytics and
 * path
 */
class JsonPathLogger : public PathLogger {
public:
  void logPath(const std::string &filename,
               const std::vector<std::pair<int, int>> &path, Metrics metrics);
};
