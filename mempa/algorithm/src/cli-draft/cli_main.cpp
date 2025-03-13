#include <iostream>
#include "cli.h"
// #include "../DemHandler/DemHandler.h"
// #include "../rover_standin/simulator.cpp"

int main(int argc, char* argv[]) {
    CLI config;

    config.parseArgs(argc, argv);

    config.validateInputs();

    if (!config.coordinate_check()) {
        std::cerr << "Error: Invalid coordinate configurations" << std::endl;
        return 1;
    }

    config.displayInputs();

    std::cout << "Program successfully initialized. Proceeding with processing..." << std::endl;

    // //DEM
    // MEMPA::BUFFDEM mars_dem(input_filepath, output_filepath);

    // config.processCoordinates();

    // if(config.start_pixel && config.end_pixel){
    //     // DEM function to convert from geographic coords to pixel coords
    //     config.pixel_coordinates.push_back(mars_dem.transformCoordinates(config.coordinates.front().first, config.coordinates.front().second));
    //     config.pixel_coordinates.push_back(mars_dem.transformCoordinates(config.coordinates.back().first, config.coordinates.back().second));
    // }

    // //Simulator
    // MEMPA::Simulator simulator("Simulator");
    // // ** can pass output file given my user ** or NULL for output into created .txt file
    // // should pass coords, DEM handler, and slope?
    // simulator.run(config.coordinates.front().first, config.coordinates.front().second, config.coordinates.back().first, config.coordinates.back().second, config.input_file);

    return 0;
}
