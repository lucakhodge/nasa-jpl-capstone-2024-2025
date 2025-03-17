#pragma once
#include <iostream>
#include <cassert>
#include <vector>
#include <queue>
#include <cfloat>
#include <algorithm>
#include<cmath>
#include <utility>

class Node;
class SearchAlgorithm;

class Node {
    public:
        int height;
        Node *previous;
        double distFromPrevious;
        double distFromNeighbor;
        bool visited;
        std::vector<int> neighborIndex;
        int selfIndex;
        int x;
        int y;
};

class SearchAlgorithm {
    public:
        SearchAlgorithm() {}
        virtual ~SearchAlgorithm() {}
        void setUpAlgo(std::vector<std::vector<float> > &heightmap, std::pair<int, int> startPoint, std::pair<int, int> endPoint, float maxSlope, float pixelSize);
        virtual std::pair<int, int> get_step() {}
        virtual void reset() {}

        // SearchAlgorithm();
        // ~SearchAlgorithm();
    protected:
    std::vector<std::vector<float>> _heightmap;
    std::pair<int, int> _startPoint;
    std::pair<int, int> _endPoint;
    double _maxSlope;
    double _pixelSize;

};