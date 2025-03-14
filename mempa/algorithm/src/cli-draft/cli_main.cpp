#include <iostream>
#include "cli.h"
// #include "../DemHandler/DemHandler.h"
// #include "../rover_standin/simulator.cpp"

/* Comment out the main function to avoid duplicate symbols
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

    return 0;
}
*/