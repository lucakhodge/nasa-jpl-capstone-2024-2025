#include <gtest/gtest.h>
#include "../src/search_algorithms/dijkstras.h"

// Test fixture for Dijkstras class
class DijkstrasTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize a simple 3x3 heightmap with float type
        heightmap = {
            {1000.0f, 1500.0f, 2000.0f},
            {1200.0f, 1300.0f, 1800.0f},
            {1100.0f, 1400.0f, 1600.0f}
        };
        dijkstra = new Dijkstras();
        // Use the public setUpAlgo method instead of direct member access
        dijkstra->setUpAlgo(heightmap, {0, 0}, {2, 2}, 45.0f, 200.0f);
    }

    void TearDown() override {
        delete dijkstra;
    }

    Dijkstras* dijkstra;
    std::vector<std::vector<float>> heightmap; // Changed to float
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
    std::vector<std::vector<float>> empty_map;
    dijkstra->setUpAlgo(empty_map, {0, 0}, {2, 2}, 45.0f, 200.0f);
    auto path = dijkstra->dijkstras();
    EXPECT_TRUE(path.empty()) << "Path should be empty for an empty heightmap";
}

// Test dijkstras() with out-of-bounds coordinates
TEST_F(DijkstrasTest, OutOfBoundsCoordinates) {
    dijkstra->setUpAlgo(heightmap, {-1, -1}, {2, 2}, 45.0f, 200.0f);
    auto path = dijkstra->dijkstras();
    EXPECT_TRUE(path.empty()) << "Path should be empty for out-of-bounds start point";

    dijkstra->setUpAlgo(heightmap, {0, 0}, {5, 5}, 45.0f, 200.0f);
    path = dijkstra->dijkstras();
    EXPECT_TRUE(path.empty()) << "Path should be empty for out-of-bounds end point";
}

// Test get_step() functionality
TEST_F(DijkstrasTest, GetStep) {
    auto step = dijkstra->get_step();
    EXPECT_NE(step, std::make_pair(-1, -1)) << "First step should not be (-1,-1)";
    
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
    EXPECT_EQ(neighbors.size(), 8u) << "Center node should have 8 neighbors"; // Use 8u to avoid sign-compare warning

    neighbors = dijkstra->get_neighbor_indexs(3, 3, 0, 0); // Top-left corner
    EXPECT_EQ(neighbors.size(), 3u) << "Corner node should have 3 neighbors"; // Use 3u
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}