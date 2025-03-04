#include <gtest/gtest.h>
#include "dijkstras.h"
#include <vector>
#include <utility>

using namespace std;

// Test fixture for Dijkstras class
class DijkstrasTest : public ::testing::Test {
protected:
    Dijkstras dijkstra;

    void SetUp() override {
        // Reset the dijkstra object before each test
        dijkstra.reset_dijkstras();
    }

    // Helper function to create a simple 3x3 heightmap
    vector<vector<double>> createSimpleHeightmap() {
        return {
            {1000.0, 1100.0, 1200.0},
            {900.0, 1000.0, 1100.0},
            {800.0, 900.0, 1000.0}
        };
    }
};

// Test empty heightmap
TEST_F(DijkstrasTest, EmptyHeightmap) {
    vector<vector<double>> heightmap;
    pair<int, int> start = {0, 0};
    pair<int, int> end = {1, 1};
    auto path = dijkstra.dijkstras(heightmap, start, end, 45.0, 200.0);
    EXPECT_TRUE(path.empty()) << "Path should be empty for an empty heightmap";
}

// Test out-of-bounds coordinates
TEST_F(DijkstrasTest, OutOfBoundsCoordinates) {
    auto heightmap = createSimpleHeightmap();
    pair<int, int> start = {-1, 0};  // Invalid start
    pair<int, int> end = {1, 1};
    auto path = dijkstra.dijkstras(heightmap, start, end, 45.0, 200.0);
    EXPECT_TRUE(path.empty()) << "Path should be empty for out-of-bounds start";

    start = {0, 0};
    end = {3, 3};  // Invalid end
    path = dijkstra.dijkstras(heightmap, start, end, 45.0, 200.0);
    EXPECT_TRUE(path.empty()) << "Path should be empty for out-of-bounds end";
}

// Test simple pathfinding
TEST_F(DijkstrasTest, SimplePathfinding) {
    auto heightmap = createSimpleHeightmap();
    pair<int, int> start = {0, 0};
    pair<int, int> end = {2, 2};
    auto path = dijkstra.dijkstras(heightmap, start, end, 45.0, 200.0);

    EXPECT_FALSE(path.empty()) << "Path should not be empty for valid input";
    EXPECT_EQ(path.front(), start) << "Path should start at the start point";
    EXPECT_EQ(path.back(), end) << "Path should end at the end point";

    // Check path continuity (each step is adjacent)
    for (size_t i = 1; i < path.size(); i++) {
        int dy = abs(path[i].first - path[i-1].first);
        int dx = abs(path[i].second - path[i-1].second);
        EXPECT_LE(dy, 1) << "Y-coordinate jump too large at step " << i;
        EXPECT_LE(dx, 1) << "X-coordinate jump too large at step " << i;
        EXPECT_TRUE(dx + dy > 0) << "No movement at step " << i;
    }
}

// Test unreachable destination due to slope constraint
TEST_F(DijkstrasTest, UnreachableDueToSlope) {
    vector<vector<double>> heightmap = {
        {1000.0, 1000.0, 1000.0},
        {1000.0, 1000.0, 6000.0},  // Steep elevation change
        {1000.0, 1000.0, 1000.0}
    };
    pair<int, int> start = {0, 0};
    pair<int, int> end = {1, 2};
    auto path = dijkstra.dijkstras(heightmap, start, end, 20.0, 200.0);  // Max slope 20°

    EXPECT_TRUE(path.empty()) << "Path should be empty when slope exceeds maximum";
}

// Test neighbor index calculation
TEST_F(DijkstrasTest, GetNeighborIndexes) {
    vector<int> neighbors = dijkstra.get_neighbor_indexs(3, 3, 1, 1);
    EXPECT_EQ(neighbors.size(), 8) << "Center node should have 8 neighbors";

    neighbors = dijkstra.get_neighbor_indexs(3, 3, 0, 0);
    EXPECT_EQ(neighbors.size(), 3) << "Corner node should have 3 neighbors";

    neighbors = dijkstra.get_neighbor_indexs(3, 3, 0, 1);
    EXPECT_EQ(neighbors.size(), 5) << "Edge node should have 5 neighbors";
}

// Test distance calculation
TEST_F(DijkstrasTest, CalculateDistanceBetweenNodes) {
    Node node1{0, 0, 0, 1000.0, {}, nullptr};
    Node node2{1, 1, 0, 1100.0, {}, nullptr};  // Diagonal move
    double rise = 100.0;
    double dist = dijkstra.calculate_distance_between_nodes(&node1, &node2, rise, 200.0);
    double expected = sqrt(pow(200.0 * M_SQRT2, 2) + pow(rise, 2));
    EXPECT_NEAR(dist, expected, 0.01) << "Diagonal distance calculation incorrect";
}

// Main function for running tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}