#include <iostream>
#include <cassert>
#include <vector>
#include <queue>
#include <cfloat>
#include <algorithm>
#include<cmath>
#include <utility>

class Node;

int calcFlatIndex(int cols, int row, int col);
std::vector<int> getNeighborIndexs(int rows, int cols, int row, int col);
std::vector<std::pair<int, int>> dijkstras(std::vector<std::vector<double> > &heightmap, int startPoint[2], int endPoint[2], double maxSlope, double pixelSize);
void pathTrace(Node finalNode);
std::pair<int, int> dijkstrasStepByStep(std::vector<std::vector<double> > &heightmap, int startPoint[2], int endPoint[2], double maxSlope, double pixelSize, std::vector<std::pair<int, int>> *pathStorage);
std::vector<std::pair<int, int>> pathToList(Node finalNode);