#include <iostream>
#include <cassert>
#include <vector>
#include <queue>
#include <cfloat>
#include <algorithm>
#include<cmath>

class Node;

int calcFlatIndex(int cols, int row, int col);
std::vector<int> getNeighborIndexs(int rows, int cols, int row, int col);
int dijkstras(std::vector<std::vector<int> > &heightmap, int startPoint[2], int endPoint[2], double maxSlope, double pixelSize);
void pathTrace(Node finalNode);