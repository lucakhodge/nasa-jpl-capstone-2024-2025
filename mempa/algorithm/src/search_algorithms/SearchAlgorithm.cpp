#include "SearchAlgorithm.h"

void SearchAlgorithm::setUpAlgo(std::vector<std::vector<float>>& heightmap, 
                                std::pair<int, int> startPoint, 
                                std::pair<int, int> endPoint, 
                                float maxSlope, 
                                float pixelSize) {
    _heightmap = heightmap;
    _startPoint = startPoint;
    _endPoint = endPoint;
    _maxSlope = maxSlope;
    _pixelSize = pixelSize;
}

// SearchAlgorithm::SearchAlgorithm()
// {
    
// }

// SearchAlgorithm::~SearchAlgorithm()
// {
    
// }