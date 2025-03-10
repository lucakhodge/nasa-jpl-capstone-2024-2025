#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include "../search_algorithms/dijkstras.h"

// Simple function to create a test heightmap
std::vector<std::vector<double>> createTestHeightmap(int width, int height) {
    std::vector<std::vector<double>> heightmap(height, std::vector<double>(width, 0.0));
    
    // Create a simple terrain with a hill in the middle
    int centerX = width / 2;
    int centerY = height / 2;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Distance from center
            double dist = std::sqrt(std::pow(x - centerX, 2) + std::pow(y - centerY, 2));
            
            // Create a gaussian hill
            if (dist < width / 3) {
                heightmap[y][x] = 100.0 * std::exp(-dist * dist / (width * width / 16.0));
            }
        }
    }
    
    return heightmap;
}

int main(int argc, char** argv) {
    std::string demFile;
    std::pair<int, int> start = {0, 0};
    std::pair<int, int> end = {0, 0};
    double maxSlope = 15.0;
    
    // Parse arguments
    for (int i = 1; i < argc; i += 2) {
        if (i + 1 >= argc) break;
        
        std::string arg = argv[i];
        if (arg == "--dem") {
            demFile = argv[i + 1];
        } 
        else if (arg == "--start") {
            std::string coords = argv[i + 1];
            size_t commaPos = coords.find(',');
            if (commaPos != std::string::npos) {
                start.first = std::stoi(coords.substr(0, commaPos));
                start.second = std::stoi(coords.substr(commaPos + 1));
            }
        }
        else if (arg == "--end") {
            std::string coords = argv[i + 1];
            size_t commaPos = coords.find(',');
            if (commaPos != std::string::npos) {
                end.first = std::stoi(coords.substr(0, commaPos));
                end.second = std::stoi(coords.substr(commaPos + 1));
            }
        }
        else if (arg == "--slope") {
            maxSlope = std::stod(argv[i + 1]);
        }
    }
    
    try {
        // Instead of loading from GDAL, create a test heightmap
        double pixelSize = 1.0;
        int width = 100, height = 100;
        
        std::vector<std::vector<double>> heightmap = createTestHeightmap(width, height);
        
        // Make sure start and end are within bounds
        start.first = std::max(0, std::min(start.first, height - 1));
        start.second = std::max(0, std::min(start.second, width - 1));
        end.first = std::max(0, std::min(end.first, height - 1));
        end.second = std::max(0, std::min(end.second, width - 1));
        
        // Run Dijkstra's algorithm
        Dijkstras dijkstras;
        auto path = dijkstras.dijkstras(heightmap, start, end, maxSlope, pixelSize);
        
        // Calculate metrics
        double pathLength = 0.0;
        double totalSlope = 0.0;
        double maxPathSlope = 0.0;
        
        for (size_t i = 1; i < path.size(); ++i) {
            double dx = path[i].second - path[i-1].second;
            double dy = path[i].first - path[i-1].first;
            double dh = heightmap[path[i].first][path[i].second] - 
                        heightmap[path[i-1].first][path[i-1].second];
                        
            double dist2D = std::sqrt(dx*dx + dy*dy) * pixelSize;
            double dist3D = std::sqrt(dist2D*dist2D + dh*dh);
            pathLength += dist3D;
            
            double slope = std::abs(std::atan2(dh, dist2D) * 180.0 / M_PI);
            totalSlope += slope;
            maxPathSlope = std::max(maxPathSlope, slope);
        }
        
        double avgSlope = path.size() > 1 ? totalSlope / (path.size() - 1) : 0.0;
        
        // Output JSON
        std::cout << "{\"path\":[";
        for (size_t i = 0; i < path.size(); ++i) {
            if (i > 0) std::cout << ",";
            std::cout << "[" << path[i].first << "," << path[i].second << "]";
        }
        std::cout << "],\"metrics\":{";
        std::cout << "\"length\":" << pathLength << ",";
        std::cout << "\"steps\":" << path.size() << ",";
        std::cout << "\"maxSlope\":" << maxPathSlope << ",";
        std::cout << "\"avgSlope\":" << avgSlope;
        std::cout << "}}" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}