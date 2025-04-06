#pragma once
#include <algorithm>
#include <cassert>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <queue>
#include <utility>
#include <vector>

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
  SearchAlgorithm() noexcept;
  virtual ~SearchAlgorithm() = default;

  virtual std::vector<std::pair<int, int>> dijkstras() = 0; // Add this line
  virtual bool can_get_next_step() = 0; // Add this line
  virtual bool is_path_storage_empty() = 0; // Add this line

  virtual std::pair<int, int>
  get_step(std::vector<std::vector<float>> &heightmap,
           std::pair<int, int> chunkLocation, std::pair<int, int> startPoint,
           std::pair<int, int> endPoint, float maxSlope, float pixelSize) = 0;
  virtual void reset() {}

  // Getters
  std::vector<std::vector<float>> getHeightmap() const { return _heightmap; }
  std::pair<int, int> getStartPoint() const { return _startPoint; }
  std::pair<int, int> getEndPoint() const { return _endPoint; }
  double getMaxSlope() const { return _maxSlope; }
  double getPixelSize() const { return _pixelSize; }

  // Setters
  void setHeightmap(const std::vector<std::vector<float>> &heightmap) {
    _heightmap = heightmap;
  }
  void setStartPoint(const std::pair<int, int> &startPoint) {
    _startPoint = startPoint;
  }
  void setEndPoint(const std::pair<int, int> &endPoint) {
    _endPoint = endPoint;
  }
  void setMaxSlope(double maxSlope) { _maxSlope = maxSlope; }
  void setPixelSize(double pixelSize) { _pixelSize = pixelSize; }

protected:
  std::vector<std::vector<float>> _heightmap;
  std::pair<int, int> _chunkLocaiton;
  std::pair<int, int> _startPoint;
  std::pair<int, int> _endPoint;
  double _maxSlope;
  double _pixelSize;
};
