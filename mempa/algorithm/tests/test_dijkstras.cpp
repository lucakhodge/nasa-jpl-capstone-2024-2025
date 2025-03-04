#include <gtest/gtest.h>
#include "dijkstras.h"

// Test fixture for Dijkstra's tests
class DijkstrasTest : public ::testing::Test {
protected:
    Dijkstras dijkstra;

    void SetUp() override {
        // Reset the Dijkstra object before each test
        dijkstra.reset_dijkstras();
    }

    // Helper function to create a simple heightmap
    std::vector<std::vector<double>> createHeightmap(int rows, int cols, double value) {
        return std::vector<std::vector<double>>(rows, std::vector<double>(cols, value));
    }
};

// Test case: Empty heightmap
TEST_F(DijkstrasTest, EmptyHeightmap) {
    std::vector<std::vector<double>> heightmap;
    std::pair<int, int> start = {0, 0};
    std::pair<int, int> end = {1, 1};
    double maxSlope = 45.0;
    double pixelSize = 200.0;

    auto path = dijkstra.dijkstras(heightmap, start, end, maxSlope, pixelSize);
    EXPECT_TRUE(path.empty()) << "Path should be empty for an empty heightmap";
}

// Test case: Valid path on flat terrain
TEST_F(DijkstrasTest, FlatTerrainPath) {
    int rows = 3, cols = 3;
    auto heightmap = createHeightmap(rows, cols, 1000.0); // Flat terrain at 1000mm (1m)
    std::pair<int, int> start = {0, 0};
    std::pair<int, int> end = {2, 2};
    double maxSlope = 45.0;
    double pixelSize = 200.0;

    auto path = dijkstra.dijkstras(heightmap, start, end, maxSlope, pixelSize);

    // Check if path is not empty
    EXPECT_FALSE(path.empty()) << "Path should exist on flat terrain";

    // Check if path starts and ends at the correct points
    EXPECT_EQ(path.front(), start) << "Path should start at (0,0)";
    EXPECT_EQ(path.back(), end) << "Path should end at (2,2)";

    // Check path length (Manhattan distance should be 4 steps: (0,0) -> (1,1) -> (2,2) or similar)
    EXPECT_GE(path.size(), 3) << "Path should have at least 3 steps";
}

// Test case: Out-of-bounds coordinates
TEST_F(DijkstrasTest, OutOfBoundsCoordinates) {
    auto heightmap = createHeightmap(3, 3, 1000.0);
    std::pair<int, int> start = {-1, 0}; // Out of bounds
    std::pair<int, int> end = {1, 1};
    double maxSlope = 45.0;
    double pixelSize = 200.0;

    auto path = dijkstra.dijkstras(heightmap, start, end, maxSlope, pixelSize);
    EXPECT_TRUE(path.empty()) << "Path should be empty for out-of-bounds start point";
}

// Test case: Steep slope exceeding maxSlope
TEST_F(DijkstrasTest, SteepSlopeNoPath) {
    std::vector<std::vector<double>> heightmap = {
        {1000.0, 1000.0},
        {1000.0, 5000.0} // Steep rise: 4m over 200m exceeds 45°
    };
    std::pair<int, int> start = {0, 0};
    std::pair<int, int> end = {1, 1};
    double maxSlope = 1.0; // Very low max slope to force no path
    double pixelSize = 200.0;

    auto path = dijkstra.dijkstras(heightmap, start, end, maxSlope, pixelSize);
    EXPECT_TRUE(path.empty()) << "No path should exist due to steep slope";
}

// Test case: Diagonal path on flat terrain
TEST_F(DijkstrasTest, DiagonalPath) {
    auto heightmap = createHeightmap(2, 2, 1000.0);
    std::pair<int, int> start = {0, 0};
    std::pair<int, int> end = {1, 1};
    double maxSlope = 45.0;
    double pixelSize = 200.0;

    auto path = dijkstra.dijkstras(heightmap, start, end, maxSlope, pixelSize);
    EXPECT_FALSE(path.empty()) << "Path should exist for diagonal movement";
    EXPECT_EQ(path.front(), start) << "Path should start at (0,0)";
    EXPECT_EQ(path.back(), end) << "Path should end at (1,1)";
    EXPECT_EQ(path.size(), 2) << "Diagonal path should be 2 steps";
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}