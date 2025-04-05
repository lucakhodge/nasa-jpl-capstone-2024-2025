#include "SearchContext.hpp"

/* We can just use a constructor instead of needing a setStrategy function. */
SearchContext::SearchContext(SearchAlgorithm *algorithmStrategy)
    : strategy(algorithmStrategy) {}

void SearchContext::setStrategy(SearchAlgorithm *strategy) {
  this->strategy = strategy;
}

std::pair<int, int> SearchContext::executeStrategyGetStep(
    std::vector<std::vector<float>> &heightmap,
    std::pair<int, int> chunkLocation, std::pair<int, int> startPoint,
    std::pair<int, int> endPoint, float maxSlope, float pixelSize) {
  return strategy->get_step(heightmap, chunkLocation, startPoint, endPoint,
                            maxSlope, pixelSize);
}

void SearchContext::executeStrategyReset() { strategy->reset(); }

void SearchContext::executeStrategySetUpAlgo(
    std::vector<std::vector<float>> &heightmap,
    std::pair<int, int> chunkLocation, std::pair<int, int> startPoint,
    std::pair<int, int> endPoint, float maxSlope, float pixelSize) {
  strategy->setUpAlgo(heightmap, chunkLocation, startPoint, endPoint, maxSlope,
                      pixelSize);
}
