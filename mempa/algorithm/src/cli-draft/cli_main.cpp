#include "../DemHandler/DemHandler.hpp"
#include "../RoverSimulator/simulator.cpp"
#include "cli.h"
#include <iostream>
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
    // print outPath in JSON format to file for GUI to read
    nlohmann::json j;
    j["data"] = nlohmann::json::array();

    // convert pair<int, int> in outPath to JSON format
    for (const auto &point : outPath) {
      j["data"].push_back({{"x", point.first}, {"y", point.second}});
    }

    std::ofstream jsonFile("rover_path.json");
    if (jsonFile.is_open()) {
      jsonFile << j.dump();
      jsonFile.close();
      std::cout << "Path saved to rover_path.json" << std::endl;
    } else {
      std::cerr << "Error: Could not open file to write JSON." << std::endl;
    }
  }
  return 0;
}
