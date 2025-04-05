#include "PathLogger.hpp"
#include "JsonPathLogger.hpp"
#include "TextPathLogger.hpp"
#include <memory>

std::unique_ptr<PathLogger> PathLogger::createLogger(bool useJson) {
  if (useJson) {
    return std::make_unique<JsonPathLogger>();
  } else {
    return std::make_unique<TextPathLogger>();
  }
}
