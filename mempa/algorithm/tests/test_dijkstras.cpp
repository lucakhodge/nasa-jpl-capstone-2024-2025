#include <gtest/gtest.h>
#include "../src/search_algorithms/dijkstras.h"

// Test fixture for Dijkstras class
class DijkstrasTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize a 3x3 heightmap for testing
        heightmap = {
            {1000.0, 2000.0, 3000.0},
            {4000.0, 5000.0, 6000.0},
            {7000.0, 8000.0, 9000.0}
        };
        dijkstra = new Dijkstras();
        dijkstra->_heightmap = heightmap;
        dijkstra->_startPoint = {0, 0};
        dijkstra->_endPoint = {2, 2};
        dijkstra->_maxSlope = 45.0; // Reasonable max slope for testing
    }

    void TearDown() override {
        delete dijkstra;
    }

    std::vector<std::vector<double>> heightmap;
    Dijkstras* dijkstra;
};

// Test empty heightmap
TEST_F(DijkstrasTest, EmptyHeightmap) {
    dijkstra->_heightmap.clear();
    auto path = dijkstra->dijkstras();
    EXPECT_TRUE(path.empty());
}

// Test invalid coordinates
TEST_F(DijkstrasTest, InvalidCoordinates) {
    dijkstra->_startPoint = {-1, 0};
    auto path = dijkstra->dijkstras();
    EXPECT_TRUE(path.empty());

    dijkstra->_startPoint = {0, 0};
    dijkstra->_endPoint = {3, 3};
    path = dijkstra->dijkstras();
    EXPECT_TRUE(path.empty());
}

// Test simple pathfinding
TEST_F(DijkstrasTest, SimplePath) {
    auto path = dijkstra->dijkstras();
    EXPECT_FALSE(path.empty());
    EXPECT_EQ(path.front(), std::make_pair(2, 2)); // End point
    EXPECT_EQ(path.back(), std::make_pair(0, 0));  // Start point
}

// Test get_step functionality
TEST_F(DijkstrasTest, GetStep) {
    auto first_step = dijkstra->get_step();
    EXPECT_EQ(first_step, std::make_pair(0, 0)); // Should be the start point initially

    // Run dijkstras to populate pathStorage
    dijkstra->dijkstras();
    dijkstra->reset_dijkstras();
    first_step = dijkstra->get_step();
    EXPECT_NE(first_step, std::make_pair(-1, -1)); // Should get a valid step
}

// Test reset functionality
TEST_F(DijkstrasTest, Reset) {
    dijkstra->dijkstras(); // Populate pathStorage
    EXPECT_FALSE(dijkstra->is_path_storage_empty());
    dijkstra->reset_dijkstras();
    EXPECT_TRUE(dijkstra->is_path_storage_empty());
    EXPECT_TRUE(dijkstra->can_get_next_step());
}

// int main(int argc, char **argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }