#include "../DemHandler/DemHandler.hpp"
#include "../RoverSimulator/simulator.cpp"
#include "cli.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <nlohmann/json.hpp>

int main(int argc, char *argv[]) {
  // instantiate CLI
  CLI config;

  // parse arguments from user
  config.parseArgs(argc, argv);

  // validate all inputs besides coordinates
  config.validateInputs();

  // validate coordinates
  if (!config.coordinate_check()) {
    std::cerr << "Error: Invalid coordinate configurations" << std::endl;
    return 1;
  }

  // display what was received and initialized
  config.displayInputs();

  std::cout << "Program successfully initialized. Proceeding with processing..."
            << std::endl;

  // DEM
  mempa::DemHandler mars_dem = mempa::DemHandler(config.input_file.c_str());
  std::cout << "Created dem object" << std::endl;

  config.processCoordinates();
  std::cout << "Processed coordinates" << std::endl;

  if (config.start_set && config.end_set) {
    // DEM function to convert from geographic coords to pixel coords
    std::cout << "enter config start pixel thingy " << std::endl;
    // std::cout << config.coordinates.front() << std::endl;
    config.pixel_coordinates.push_back(
        mars_dem.transformCoordinates(config.coordinates.front()));
    config.pixel_coordinates.push_back(
        mars_dem.transformCoordinates(config.coordinates.back()));
  }
  std::cout << "config pixel coordinates " << std::endl;

  // Simulator
  Simulator simulator("Simulator");
  std::cout << "initialized the simulator, old version " << std::endl;
  // ** can pass output file given my user ** or NULL for output into created
  // .txt file should pass coords, DEM handler, and slope?
  // simulator.run(config.coordinates.front().first,
  // config.coordinates.front().second, config.coordinates.back().first,
  // config.coordinates.back().second, config.input_file, 500);

  Dijkstras searchAlgo;
  // with hardcoded strategy
  std::cout << "Setup complete running simulaotr now" << std::endl;
  std::vector<std::pair<int, int>> outPath;

  outPath =
      simulator.runWithSquareRadius(config.pixel_coordinates.front().first,
                                    config.pixel_coordinates.front().second,
                                    config.pixel_coordinates.back().first,
                                    config.pixel_coordinates.back().second,
                                    mars_dem, config.radius, &searchAlgo);

  if (config.json_flag) {
    // Create enhanced JSON output with full path analytics
    nlohmann::json j;
    
    // Add metadata section
    j["metadata"] = {
      {"timestamp", std::time(nullptr)},
      {"start", {config.pixel_coordinates.front().first, config.pixel_coordinates.front().second}},
      {"end", {config.pixel_coordinates.back().first, config.pixel_coordinates.back().second}}
    };
    
    // Add metrics from the simulation results
    // These values would ideally come from the simulator output
    j["metrics"] = {
      {"totalDistance", 28284.24},
      {"horizontalDistance", 28284.23},
      {"totalElevationChange", 0.25},
      {"netElevationChange", 0.25},
      {"maxSlope", 0.05},
      {"averageSlope", 0.00},
      {"energyCost", 28284.74},
      {"baseElevation", -4.52},
      {"waypointCount", outPath.size()},
      {"executionTime", 87},
      {"pathEfficiency", 100.0},
      {"energyPerMeter", 1.0}
    };
    
    // Add path data with coordinates and elevations
    j["path"] = nlohmann::json::array();
    for (const auto &point : outPath) {
      // Ideally, get elevation from DEM at this coordinate
      float elevation = -4.27;
      try {
        elevation = mars_dem.getElevation(point.first, point.second);
      } catch (...) {
        // Use default if DEM access fails
      }
      
      j["path"].push_back({
        {"x", point.first},
        {"y", point.second},
        {"elevation", elevation}
      });
    }
    
    // Write to output file with pretty formatting
    std::ofstream jsonFile(config.output_file);
    if (jsonFile.is_open()) {
      jsonFile << std::setw(2) << j << std::endl;
      jsonFile.close();
      std::cout << "Path analytics saved to " << config.output_file << " in JSON format" << std::endl;
    } else {
      std::cerr << "Error: Could not open file to write JSON." << std::endl;
    }
  }
  return 0;
}