#include "SearchAlgorithm.hpp"

void SearchAlgorithm::setUpAlgo(std::vector<std::vector<float>> &heightmap,
                                std::pair<int, int> chunkLocation,
                                std::pair<int, int> startPoint,
                                std::pair<int, int> endPoint, float maxSlope,
                                float pixelSize) {
  _heightmap = heightmap;
  _chunkLocaiton = chunkLocation;
  _startPoint = startPoint;
  _endPoint = endPoint;
  _maxSlope = maxSlope;
  _pixelSize = pixelSize;
}

SearchAlgorithm::SearchAlgorithm() {}