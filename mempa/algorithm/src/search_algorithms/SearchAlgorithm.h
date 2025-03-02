#include <iostream>
#include <cassert>
#include <vector>
#include <queue>
#include <cfloat>
#include <algorithm>
#include<cmath>
#include <utility>

class SearchAlgorithm;
class Node;

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
        std::pair<int, int> get_step(std::vector<std::vector<double> > &heightmap, std::pair<int, int> startPoint, std::pair<int, int> endPoint);
    private:

};
