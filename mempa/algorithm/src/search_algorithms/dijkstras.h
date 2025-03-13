#include "SearchAlgorithm.h"

class Dijkstras;

class Dijkstras : public SearchAlgorithm {
    public:
        std::vector<std::pair<int, int>> dijkstras();//std::vector<std::vector<double> > &heightmap, std::pair<int, int> startPoint, std::pair<int,int> endPoint, double maxSlope, double pixelSize);
        std::pair<int, int> get_step();//std::vector<std::vector<double> > &heightmap, std::pair<int, int> startPoint, std::pair<int, int> endPoint, double maxSlope, double pixelSize);//, std::vector<std::pair<int, int>> *pathStorage);
        void reset_dijkstras();
        bool can_get_next_step();
        bool is_path_storage_empty();

    // private:
        std::vector<std::pair<int, int>> pathStorage;
        bool pathStoredThanDisplayed = false;
        bool compare_nodes_by_height(Node* node1, Node* node2);
        static bool compare_nodes_by_dist_from_neighbor(Node* node1, Node* node2);
        int calc_flat_index(int cols, int row, int col);
        double calculate_distance_between_nodes(Node* node1, Node* node2, double rise, double pixelSize);
        std::vector<int> get_neighbor_indexs(int rows, int cols, int row, int col);
        void path_trace(Node finalNode);
        std::vector<std::pair<int, int>> path_to_list(Node finalNode);

};

// int calc_flat_index(int cols, int row, int col);
// std::vector<int> get_neighbor_indexs(int rows, int cols, int row, int col);
// //std::vector<std::pair<int, int>> dijkstras(std::vector<std::vector<double> > &heightmap, std::pair<int, int> startPoint, std::pair<int, int> endPoint, double maxSlope, double pixelSize);
// void path_trace(Node finalNode);
// //std::pair<int, int> dijkstrasStepByStep(std::vector<std::vector<double> > &heightmap, std::pair<int, int> startPoint, std::pair<int, int> endPoint, double maxSlope, double pixelSize, std::vector<std::pair<int, int>> *pathStorage);
// std::vector<std::pair<int, int>> path_to_list(Node finalNode);