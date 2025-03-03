#include"SearchAlgorithm.h"

void SearchAlgorithm::setUpAlgo(std::vector<std::vector<double> > &heightmap, std::pair<int, int> startPoint, std::pair<int, int> endPoint, double maxSlope, double pixelSize) 
{
    _heightmap = heightmap;
    _startPoint = startPoint;
    _endPoint = endPoint;
    _maxSlope = maxSlope;
    _pixelSize = pixelSize;
}