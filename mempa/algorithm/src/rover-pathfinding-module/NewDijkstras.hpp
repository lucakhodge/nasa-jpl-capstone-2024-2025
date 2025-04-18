#pragma once
#include "SearchAlgorithm.hpp"
#include <utility>
#include <vector>
#include <queue>

class NewDijkstras : public SearchAlgorithm {
public:
    virtual ~NewDijkstras() override = default; // Add virtual destructor
    std::vector<std::pair<int, int>> get_step(std::vector<std::vector<float>> &heightmap,
        std::pair<int, int> chunkLocation, std::pair<int, int> startPoint,
        std::pair<int, int> endPoint, float maxSlope, float pixelSize) override;
    std::vector<std::pair<int, int>> newDijkstras();
    int calc_flat_index(int cols, int row, int col);
    std::vector<int> get_neighbor_indexs(int rows, int cols, int row, int col);
    std::vector<std::pair<int, int>> path_to_list(Node finalNode);
    static bool compare_nodes_by_dist_from_neighbor(Node* node1, Node* node2);
    double calculate_distance_between_nodes(Node* node1, Node* node2, double rise, double pixelSize);
};