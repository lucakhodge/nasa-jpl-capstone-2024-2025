#pragma once
#include <iostream>
#include "SearchAlgorithm.h"

class SearchContext;

class SearchContext 
{
    private:
        SearchAlgorithm* strategy;

    public:
        void setStrategy(SearchAlgorithm* strategy);
        std::pair<int, int> executeStrategyGetStep();
        void executeStrategyReset();
        void executeStrategySetUpAlgo(std::vector<std::vector<float> > &heightmap, std::pair<int, int> startPoint, std::pair<int, int> endPoint, float maxSlope, float pixelSize);
};