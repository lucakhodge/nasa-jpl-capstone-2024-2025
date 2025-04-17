#include "SearchAlgorithm.hpp"

SearchAlgorithm::SearchAlgorithm() noexcept {}

/**
 * @brief non-default way to set up internal parameters to be used with a SearchAlgorithm subclass
 * 
 * @param heightmap contains the height values to be used for naviagtion, Usualy a chunk of a larger heightmap
 * @param chunkLocation 0,0 in the passed heightmap is this value in the whole larger heightmap (global context)
 * @param startPoint the start point for navigation in the whole larger heightmap (global context)
 * @param endPoint the end point for nagivation in the whole larger heightmap (global context)
 * @param maxSlope the maximum slope that is allowed to be navigated over
 * @param pixelSize the size (in meters) of the resolution of the heightmap
 */
void SearchAlgorithm::setUpAlgo(std::vector<std::vector<float>> &heightmap,
                                std::pair<int, int> chunkLocation,
                                std::pair<int, int> startPoint,
                                std::pair<int, int> endPoint, float maxSlope,
                                float pixelSize)
{
  _heightmap = heightmap;
  _chunkLocaiton = chunkLocation;
  _startPoint = startPoint;
  _endPoint = endPoint;
  _maxSlope = maxSlope;
  _pixelSize = pixelSize;
}
