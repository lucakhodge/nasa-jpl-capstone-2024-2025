#include "dijkstras.h"
#include <iostream>
#include <queue>
#include <vector>
#include <limits>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <unordered_set>
#include <algorithm>

using namespace std;

// Constants for Mars terrain analysis
const double MIN_ELEVATION = -8500.0;    // Hellas Basin (meters)
const double MAX_ELEVATION = 22500.0;    // Olympus Mons (meters)
const double MM_TO_M = 0.001;            // Millimeter to meter conversion
const double NO_DATA_VALUE = -32768.0;   // Common no-data value for DEMs
const double MAX_SLOPE = 45.0;           // Maximum slope in degrees
const double MAX_ELEVATION_CHANGE = 5.0; // Maximum elevation change per pixel (meters)
const double ELEVATION_EPSILON = 0.01;   // Small elevation difference threshold (meters)
const double PIXEL_SIZE = 200.0;         // Mars DEM resolution (meters)


// Elevation tracking structure
struct ElevationTracker {
    double baseElevation;
    vector<vector<double>> relativeHeights;
    int rows;
    int cols;
    int warningCount;

    ElevationTracker() : baseElevation(std::numeric_limits<double>::quiet_NaN()),
                        warningCount(0), rows(0), cols(0) {}

    void initialize(int r, int c) {
        rows = r;
        cols = c;
        relativeHeights.resize(rows, vector<double>(cols, 0.0));
    }

    void reset() {
        baseElevation = std::numeric_limits<double>::quiet_NaN();
        warningCount = 0;
        relativeHeights.clear();
    }

    double processElevation(double rawElevation, int row, int col) {
        if (rawElevation == NO_DATA_VALUE || std::isnan(rawElevation)) {
            return 0.0;
        }

        double absoluteElevation = rawElevation * MM_TO_M;

        if (std::isnan(baseElevation)) {
            baseElevation = absoluteElevation;
            if (warningCount++ < 5) {
                cout << "Base elevation: " << fixed << setprecision(2) 
                     << baseElevation << "m" << endl;
            }
            return 0.0;
        }

        double relativeElevation = absoluteElevation - baseElevation;
        relativeHeights[row][col] = relativeElevation;

        if (warningCount++ < 5) {
            cout << fixed << setprecision(2)
                 << "At (" << row << "," << col << "): "
                 << "Raw=" << rawElevation << "mm, "
                 << "Abs=" << absoluteElevation << "m, "
                 << "Rel=" << relativeElevation << "m" << endl;
        }

        return relativeElevation;
    }
};

static ElevationTracker elevTracker;

struct CompareNodes {
    bool operator()(pair<double, Node*> const& p1, pair<double, Node*> const& p2) {
        return p1.first > p2.first;
    }
};

std::vector<std::pair<int, int>> Dijkstras::dijkstras()
{
    elevTracker.reset();
    
    if (_heightmap.empty() || _heightmap[0].empty()) {
        cout << "Error: Empty heightmap provided" << endl;
        return {};
    }

    int rows = _heightmap.size();
    int cols = _heightmap[0].size();
    elevTracker.initialize(rows, cols);

    // Validate coordinates
    if (_startPoint.first < 0 || _startPoint.first >= rows ||
        _startPoint.second < 0 || _startPoint.second >= cols ||
        _endPoint.first < 0 || _endPoint.first >= rows ||
        _endPoint.second < 0 || _endPoint.second >= cols) {
        cout << "Dijkstras Error: Start or end point out of bounds" << endl;
        return {};
    }

    vector<Node> graph(rows * cols);
    vector<bool> visited(rows * cols, false);
    priority_queue<pair<double, Node*>, vector<pair<double, Node*>>, CompareNodes> pq;

    // Initialize graph
    cout << "Initializing graph with " << rows << "x" << cols << " nodes..." << endl;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int index = calc_flat_index(cols, row, col);
            graph[index].height = elevTracker.processElevation(_heightmap[row][col], row, col);
            graph[index].neighborIndex = get_neighbor_indexs(rows, cols, row, col);
            graph[index].selfIndex = index;
            graph[index].distFromPrevious = DBL_MAX;
            graph[index].x = col;
            graph[index].y = row;
        }
    }

    // Set start node
    int startIndex = calc_flat_index(cols, _startPoint.first, _startPoint.second);
    graph[startIndex].distFromPrevious = 0;
    pq.push({0, &graph[startIndex]});

    cout << "Starting pathfinding from (" << _startPoint.first << "," << _startPoint.second 
         << ") to (" << _endPoint.first << "," << _endPoint.second << ")" << endl;

    int iteration = 0;
    while (!pq.empty()) {
        Node* current = pq.top().second;
        pq.pop();

        if (visited[current->selfIndex]) continue;
        visited[current->selfIndex] = true;

        // Check destination
        if (current->x == _endPoint.second && current->y == _endPoint.first) {
            cout << "Path found after " << iteration << " iterations" << endl;
            return path_to_list(graph[calc_flat_index(cols, _endPoint.first, _endPoint.second)]);
        }

        // Process neighbors
        for (int neighborIndex : current->neighborIndex) {
            Node* neighbor = &graph[neighborIndex];
            if (visited[neighbor->selfIndex]) continue;

            double elevChange = neighbor->height - current->height;
            bool isDiagonal = (current->x != neighbor->x && current->y != neighbor->y);
            double horizontalDist = isDiagonal ? PIXEL_SIZE * M_SQRT2 : PIXEL_SIZE;
            
            // Cap elevation change
            elevChange = std::min(std::abs(elevChange), MAX_ELEVATION_CHANGE);
            if (std::abs(elevChange) < ELEVATION_EPSILON) {
                elevChange = 0.0;
            }

            double slope = atan2(elevChange, horizontalDist) * 180.0 / M_PI;

            if (slope <= _maxSlope) {
                double distance = calculate_distance_between_nodes(current, neighbor, 
                                                                elevChange, PIXEL_SIZE);
                double newDist = current->distFromPrevious + distance;

                if (newDist < neighbor->distFromPrevious) {
                    neighbor->distFromPrevious = newDist;
                    neighbor->previous = current;
                    pq.push({newDist, neighbor});
                }
            }
        }

        if (++iteration % 1000 == 0) {
            double progress = (double)visited.size() / (rows * cols) * 100.0;
            cout << "Iteration: " << iteration 
                 << ", At: (" << current->x << "," << current->y << ")"
                 << ", Height: " << fixed << setprecision(2) << current->height << "m"
                 << ", Progress: " << progress << "%" << endl;
        }
    }

    cout << "No valid path found after " << iteration << " iterations" << endl;
    return {};
}

pair<int, int> Dijkstras::get_step()//vector<vector<double> > &heightmap, pair<int, int> startPoint, pair<int, int> endPoint, double maxSlope, double pixelSize)//, vector<pair<int, int>> *pathStorage)
{
    pair<int, int> out;

    if(pathStoredThanDisplayed)
    {
        out = {-1,-1};
        cout << "You have completed the path, if you want to start over please call reset_dijkstras(), returning -1, -1" << endl;
        return out;
    }

    if(pathStorage.empty() && pathStoredThanDisplayed == false)
    {
        vector<pair<int, int>> realPath = dijkstras();

        for(int i = 0; i < realPath.size(); i++)
        {
            pathStorage.push_back(realPath.at(i));
        }

        out = pathStorage.back();
        pathStorage.pop_back();

        return out;
    }
    else
    {
        out = pathStorage.back();
        pathStorage.pop_back();

        if(pathStorage.empty())
        {
            pathStoredThanDisplayed = true;
        }

        return out;
    }
}

vector<int> Dijkstras::get_neighbor_indexs(int rows, int cols, int row, int col) {
    vector<int> neighbors;
    neighbors.reserve(8);

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            
            int newRow = row + i;
            int newCol = col + j;
            
            if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols) {
                neighbors.push_back(calc_flat_index(cols, newRow, newCol));
            }
        }
    }

    return neighbors;
}

double Dijkstras::calculate_distance_between_nodes(Node* node1, Node* node2, 
    double rise, double pixelSize) {
    bool isDiagonal = (node1->x != node2->x && node1->y != node2->y);
    double run = isDiagonal ? (pixelSize * M_SQRT2) : pixelSize;
    return sqrt(pow(run, 2) + pow(rise, 2));
}

int Dijkstras::calc_flat_index(int cols, int row, int col) {
    return row * cols + col;
}

vector<pair<int, int>> Dijkstras::path_to_list(Node finalNode) {
    vector<pair<int, int>> path;
    const Node* current = &finalNode;

    while (current->distFromPrevious != 0) {
        path.push_back({current->y, current->x});
        current = current->previous;
    }
    path.push_back({current->y, current->x});

    //reverse(path.begin(), path.end());
    
    // Calculate path metrics
    double totalDistance = 0.0;
    double maxSlope = 0.0;
    double totalSlope = 0.0;
    int steps = path.size() - 1;
    
    for (size_t i = 0; i < path.size() - 1; i++) {
        const auto& p1 = path[i];
        const auto& p2 = path[i + 1];
        bool isDiagonal = (p1.first != p2.first && p1.second != p2.second);
        double horizontalDist = isDiagonal ? PIXEL_SIZE * M_SQRT2 : PIXEL_SIZE;
        double elevChange = std::abs(elevTracker.relativeHeights[p2.first][p2.second] - 
        elevTracker.relativeHeights[p1.first][p1.second]);
        double slope = atan2(elevChange, horizontalDist) * 180.0 / M_PI;
        
        totalDistance += sqrt(pow(horizontalDist, 2) + pow(elevChange, 2));
        maxSlope = std::max(maxSlope, slope);
        totalSlope += slope;
    }

    cout << "Path metrics:" << endl
         << "- Length: " << fixed << setprecision(2) << totalDistance << "m" << endl
         << "- Steps: " << path.size() << endl
         << "- Max slope: " << maxSlope << "°" << endl
         << "- Avg slope: " << (totalSlope / steps) << "°" << endl;
    
    return path;
}

void Dijkstras::reset_dijkstras() {
    pathStorage.clear();
    pathStoredThanDisplayed = false;
    elevTracker.reset();
}

bool Dijkstras::can_get_next_step() {
    return !pathStoredThanDisplayed;
}

bool Dijkstras::is_path_storage_empty() {
    return pathStorage.empty();
}