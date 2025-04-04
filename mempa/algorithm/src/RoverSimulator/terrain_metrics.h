// Create this file at /Users/adamcarlson/nasa-jpl-capstone-2024-2025-8/mempa/algorithm/src/RoverSimulator/terrain_metrics.h

#pragma once

/**
 * @brief Stores comprehensive terrain metrics for path analysis
 * 
 * @author Adam Carlson
 */
class TerrainMetrics {
public:
    float totalDistance;         // Total traversed distance (m)
    float horizontalDistance;    // Horizontal distance (m)
    float totalElevationChange;  // Cumulative elevation changes (m)
    float netElevationChange;    // Net elevation difference (m)
    float maxSlope;              // Maximum slope encountered (degrees)
    float averageSlope;          // Average slope throughout path (degrees)
    float energyCost;            // Energy cost estimate
    float baseElevation;         // Base elevation for reference (m)
    int waypointCount;           // Number of waypoints in path

    TerrainMetrics() :
        totalDistance(0.0f), 
        horizontalDistance(0.0f),
        totalElevationChange(0.0f),
        netElevationChange(0.0f),
        maxSlope(0.0f),
        averageSlope(0.0f),
        energyCost(0.0f),
        baseElevation(0.0f),
        waypointCount(0)
    {}
};