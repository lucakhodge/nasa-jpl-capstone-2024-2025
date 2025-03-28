#include <cassert>
#include <iostream>
#include <vector>
#include <utility>

#include "search_algorithms/dijkstras.h"

using namespace std;

// Helper function to print test results
void print_test_result(const string& test_name, bool passed) {
    cout << "Test: " << test_name << " - " << (passed ? "PASSED" : "FAILED") << endl;
}

// Test 1: Test calc_flat_index
void test_calc_flat_index() {
    Dijkstras dijkstra;
    int cols = 5;
    int row = 2;
    int col = 3;
    int expected = 2 * 5 + 3; // 13
    int result = dijkstra.calc_flat_index(cols, row, col);
    bool passed = (result == expected);
    print_test_result("calc_flat_index", passed);
    assert(passed && "calc_flat_index failed");
}

// Test 2: Test get_neighbor_indexs
void test_get_neighbor_indexs() {
    Dijkstras dijkstra;
    int rows = 3;
    int cols = 3;
    int row = 1;
    int col = 1;
    vector<int> neighbors = dijkstra.get_neighbor_indexs(rows, cols, row, col);
    
    // Expected neighbors for (1,1) in a 3x3 grid (0-based indices):
    // (0,0) -> 0, (0,1) -> 1, (0,2) -> 2
    // (1,0) -> 3,           , (1,2) -> 5
    // (2,0) -> 6, (2,1) -> 7, (2,2) -> 8
    vector<int> expected = {0, 1, 2, 3, 5, 6, 7, 8};
    bool passed = (neighbors.size() == expected.size());
    if (passed) {
        for (size_t i = 0; i < neighbors.size(); ++i) {
            if (neighbors[i] != expected[i]) {
                passed = false;
                break;
            }
        }
    }
    print_test_result("get_neighbor_indexs_center", passed);
    assert(passed && "get_neighbor_indexs_center failed");

    // Test a corner case (0,0)
    neighbors = dijkstra.get_neighbor_indexs(rows, cols, 0, 0);
    expected = {1, 3, 4}; // (0,1), (1,0), (1,1)
    passed = (neighbors.size() == expected.size());
    if (passed) {
        for (size_t i = 0; i < neighbors.size(); ++i) {
            if (neighbors[i] != expected[i]) {
                passed = false;
                break;
            }
        }
    }
    print_test_result("get_neighbor_indexs_corner", passed);
    assert(passed && "get_neighbor_indexs_corner failed");
}

// Test 3: Test dijkstras with a simple 2x2 grid
void test_dijkstras_simple() {
    Dijkstras dijkstra;
    vector<vector<double>> heightmap = {
        {0.0, 0.0},
        {0.0, 0.0}
    };
    pair<int, int> start = {0, 0};
    pair<int, int> end = {1, 1};
    double maxSlope = 45.0;
    double pixelSize = 200.0;

    // Set up the Dijkstra object
    dijkstra._heightmap = heightmap;
    dijkstra._startPoint = start;
    dijkstra._endPoint = end;
    dijkstra._maxSlope = maxSlope;
    dijkstra._pixelSize = pixelSize;

    vector<pair<int, int>> path = dijkstra.dijkstras();
    
    // Expected path: (0,0) -> (1,1) (diagonal path, since heights are the same)
    vector<pair<int, int>> expected = {{1, 1}, {0, 0}};
    bool passed = (path.size() == expected.size());
    if (passed) {
        for (size_t i = 0; i < path.size(); ++i) {
            if (path[i] != expected[i]) {
                passed = false;
                break;
            }
        }
    }
    print_test_result("dijkstras_simple_2x2", passed);
    assert(passed && "dijkstras_simple_2x2 failed");
}

// Test 4: Test dijkstras with invalid coordinates
void test_dijkstras_invalid_coords() {
    Dijkstras dijkstra;
    vector<vector<double>> heightmap = {
        {0.0, 0.0},
        {0.0, 0.0}
    };
    pair<int, int> start = {0, 0};
    pair<int, int> end = {2, 2}; // Out of bounds
    double maxSlope = 45.0;
    double pixelSize = 200.0;

    dijkstra._heightmap = heightmap;
    dijkstra._startPoint = start;
    dijkstra._endPoint = end;
    dijkstra._maxSlope = maxSlope;
    dijkstra._pixelSize = pixelSize;

    vector<pair<int, int>> path = dijkstra.dijkstras();
    bool passed = path.empty();
    print_test_result("dijkstras_invalid_coords", passed);
    assert(passed && "dijkstras_invalid_coords failed");
}

// Test 5: Test get_step functionality
void test_get_step() {
    Dijkstras dijkstra;
    vector<vector<double>> heightmap = {
        {0.0, 0.0},
        {0.0, 0.0}
    };
    pair<int, int> start = {0, 0};
    pair<int, int> end = {1, 1};
    double maxSlope = 45.0;
    double pixelSize = 200.0;

    dijkstra._heightmap = heightmap;
    dijkstra._startPoint = start;
    dijkstra._endPoint = end;
    dijkstra._maxSlope = maxSlope;
    dijkstra._pixelSize = pixelSize;

    // First call to get_step should compute the path and return the last step
    pair<int, int> step = dijkstra.get_step();
    bool passed = (step == make_pair(1, 1));
    print_test_result("get_step_first", passed);
    assert(passed && "get_step_first failed");

    // Second call should return the next step
    step = dijkstra.get_step();
    passed = (step == make_pair(0, 0));
    print_test_result("get_step_second", passed);
    assert(passed && "get_step_second failed");

    // After the path is exhausted, it should return {-1, -1}
    step = dijkstra.get_step();
    passed = (step == make_pair(-1, -1));
    print_test_result("get_step_exhausted", passed);
    assert(passed && "get_step_exhausted failed");
}

int main() {
    cout << "Running Dijkstra's tests..." << endl;
    test_calc_flat_index();
    test_get_neighbor_indexs();
    test_dijkstras_simple();
    test_dijkstras_invalid_coords();
    test_get_step();
    cout << "All tests completed." << endl;
    return 0;
}