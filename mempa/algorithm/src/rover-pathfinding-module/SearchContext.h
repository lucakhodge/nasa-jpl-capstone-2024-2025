#pragma once
#include "SearchAlgorithm.h"
#include <iostream>

class SearchContext;

class SearchContext {
private:
  SearchAlgorithm *strategy;

public:
  SearchContext(SearchAlgorithm *searchAlgorithm);
  void setStrategy(SearchAlgorithm *strategy);
  std::pair<int, int> executeStrategyGetStep(
      std::vector<std::vector<float>> &heightmap,
      std::pair<int, int> chunkLocation, std::pair<int, int> startPoint,
      std::pair<int, int> endPoint, float maxSlope, float pixelSize);
  void executeStrategyReset();
  void executeStrategySetUpAlgo(std::vector<std::vector<float>> &heightmap,
                                std::pair<int, int> chunkLocation,
                                std::pair<int, int> startPoint,
                                std::pair<int, int> endPoint, float maxSlope,
                                float pixelSize);
};
