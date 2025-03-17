#include <gtest/gtest.h>
#include "../src/search_algorithms/dijkstras.h"

// Test fixture for Dijkstras class
class DijkstrasTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize a simple 3x3 heightmap
        heightmap = {
            {1000.0, 1500.0, 2000.0},
            {1200.0, 1300.0, 1800.0},
            {1100.0, 1400.0, 1600.0}
        };
        dijkstra = new Dijkstras();
        dijkstra->_heightmap = heightmap;
        dijkstra->_startPoint = {0, 0}; // Top-left
        dijkstra->_endPoint = {2, 2};   // Bottom-right
        dijkstra->_maxSlope = 45.0;     // Reasonable max slope
    }

    void TearDown() override {
        delete dijkstra;
    }

    Dijkstras* dijkstra;
    std::vector<std::vector<double>> heightmap;
};

// Test the dijkstras() method with a valid path
TEST_F(DijkstrasTest, ValidPath) {
    auto path = dijkstra->dijkstras();
    ASSERT_FALSE(path.empty()) << "Path should not be empty";
    EXPECT_EQ(path.front(), std::make_pair(0, 0)) << "Path should start at (0,0)";
    EXPECT_EQ(path.back(), std::make_pair(2, 2)) << "Path should end at (2,2)";
}

// Test dijkstras() with an empty heightmap
TEST_F(DijkstrasTest, EmptyHeightmap) {
    dijkstra->_heightmap.clear();
    auto path = dijkstra->dijkstras();
    EXPECT_TRUE(path.empty()) << "Path should be empty for an empty heightmap";
}

// Test dijkstras() with out-of-bounds coordinates
TEST_F(DijkstrasTest, OutOfBoundsCoordinates) {
    dijkstra->_startPoint = {-1, -1};
    auto path = dijkstra->dijkstras();
    EXPECT_TRUE(path.empty()) << "Path should be empty for out-of-bounds start point";

    dijkstra->_startPoint = {0, 0};
    dijkstra->_endPoint = {5, 5};
    path = dijkstra->dijkstras();
    EXPECT_TRUE(path.empty()) << "Path should be empty for out-of-bounds end point";
}

// Test get_step() functionality
TEST_F(DijkstrasTest, GetStep) {
    // First call should populate pathStorage and return the last step
    auto step = dijkstra->get_step();
    EXPECT_NE(step, std::make_pair(-1, -1)) << "First step should not be (-1,-1)";
    
    // Keep getting steps until path is exhausted
    while (dijkstra->can_get_next_step()) {
        step = dijkstra->get_step();
    }
    EXPECT_EQ(step, std::make_pair(-1, -1)) << "Final step should be (-1,-1)";
}

// Test reset_dijkstras()
TEST_F(DijkstrasTest, ResetFunctionality) {
    dijkstra->get_step(); // Populate pathStorage
    EXPECT_FALSE(dijkstra->is_path_storage_empty()) << "Path storage should not be empty after get_step";
    
    dijkstra->reset_dijkstras();
    EXPECT_TRUE(dijkstra->is_path_storage_empty()) << "Path storage should be empty after reset";
    EXPECT_TRUE(dijkstra->can_get_next_step()) << "Should be able to get next step after reset";
}

// Test neighbor index calculation
TEST_F(DijkstrasTest, GetNeighborIndexes) {
    auto neighbors = dijkstra->get_neighbor_indexs(3, 3, 1, 1); // Center of 3x3 grid
    EXPECT_EQ(neighbors.size(), 8) << "Center node should have 8 neighbors";

    neighbors = dijkstra->get_neighbor_indexs(3, 3, 0, 0); // Top-left corner
    EXPECT_EQ(neighbors.size(), 3) << "Corner node should have 3 neighbors";
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}