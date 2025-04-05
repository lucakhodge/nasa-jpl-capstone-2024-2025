#include "PathLogger.h"
#include "JsonPathLogger.h"
#include "TextPathLogger.h"
#include <memory>

std::unique_ptr<PathLogger> PathLogger::createLogger(bool useJson) {
  if (useJson) {
    return std::make_unique<JsonPathLogger>();
  } else {
    return std::make_unique<TextPathLogger>();
  }
}
