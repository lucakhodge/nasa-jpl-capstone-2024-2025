#include "SearchContext.h"

/* We can just use a constructor instead of needing a setStrategy function. */
SearchContext::SearchContext(SearchAlgorithm *algorithmStrategy)
    : strategy(algorithmStrategy)
{
}

void SearchContext::setStrategy(SearchAlgorithm *strategy)
{
    this->strategy = strategy;
}

std::pair<int, int> SearchContext::executeStrategyGetStep()
{
    return strategy->get_step();
}

void SearchContext::executeStrategyReset()
{
    strategy->reset();
}

void SearchContext::executeStrategySetUpAlgo(std::vector<std::vector<float>> &heightmap, std::pair<int, int> startPoint, std::pair<int, int> endPoint, float maxSlope, float pixelSize)
{
    strategy->setUpAlgo(heightmap, startPoint, endPoint, maxSlope, pixelSize);
}