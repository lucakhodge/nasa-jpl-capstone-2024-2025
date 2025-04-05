#pragma once
#include "SearchAlgorithm.h"
#include <utility>
#include <vector>

class Dijkstras : public SearchAlgorithm {

public:
  Dijkstras() {}
  ~Dijkstras() {}

  std::vector<std::pair<int, int>> dijkstras();
  std::pair<int, int> get_step(std::vector<std::vector<float>> &heightmap,
                               std::pair<int, int> chunkLocation,
                               std::pair<int, int> startPoint,
                               std::pair<int, int> endPoint, float maxSlope,
                               float pixelSize);
  void reset();
  bool can_get_next_step();
  bool is_path_storage_empty();

  // Getters
  std::vector<std::pair<int, int>> get_path_storage() const {
    return pathStorage;
  }
  bool get_path_stored_than_displayed() const {
    return pathStoredThanDisplayed;
  }
  // New public getters for private methods
  int get_flat_index(int cols, int row, int col) {
    return calc_flat_index(cols, row, col);
  }
  std::vector<int> get_neighbors(int rows, int cols, int row, int col) {
    return get_neighbor_indexs(rows, cols, row, col);
  }

  // Setters
  void set_path_storage(const std::vector<std::pair<int, int>> &path) {
    pathStorage = path;
  }
  void set_path_stored_than_displayed(bool value) {
    pathStoredThanDisplayed = value;
  }

private:
  std::vector<std::pair<int, int>> pathStorage;
  bool pathStoredThanDisplayed = false;
  bool compare_nodes_by_height(Node *node1, Node *node2);
  static bool compare_nodes_by_dist_from_neighbor(Node *node1, Node *node2);
  int calc_flat_index(int cols, int row, int col);
  double calculate_distance_between_nodes(Node *node1, Node *node2, double rise,
                                          double pixelSize);
  std::vector<int> get_neighbor_indexs(int rows, int cols, int row, int col);
  void path_trace(Node finalNode);
  std::vector<std::pair<int, int>> path_to_list(Node finalNode);
};
