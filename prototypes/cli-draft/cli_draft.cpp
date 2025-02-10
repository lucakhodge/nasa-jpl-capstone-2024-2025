/*
Instructions for use:

** Make sure you are in correct directory where cli file is **
To Compile: g++ ./cli_draft.cpp -o cli

** Fill in inputs and flags with correct ones **
To run: ./cli --flag INPUT

** Example **
./cli --start 44.43,55.55 --end 32.22,55.33 --input test.dem --output test.txt --iterations 4 --slope 12.3 

Required flags:
    2 of the following (1 being start and 1 being end):
        --start
        --end
        --start_area
        --end_area 
    
    --input
    --output
    --iterations
    --slope

** Use --help for printing out input formats with each flag **


*/

#include <iostream>
#include <getopt.h>
#include <string>
#include <fstream>
#include <regex>
#include "buf_dem.h"

// print out helper menu
void print_helper() {
    std::cout << "Options:\n"
              << "  --start          Start coordinates (e.g., 1,1 or 'random')\n"
              << "  --end            End coordinates (e.g., 10,10)\n"
              << "  --start-area     Start area range (e.g., 1,1:5,5)\n"
              << "  --end-area       End area range (e.g., 1,1:5,5)\n"
              << "  --input          Input DEM file\n"
              << "  --output         Output results file\n"
              << "  --iterations     Number of iterations\n"
              << "  --slope          Slope tolerances (e.g., 10,20,30)\n"
              << "  --radius         Visibility Radius of Rover (in meters)\n"
              << "  --help           Print help message\n";
}

// split a string into x and y coordinates (for specific coordinates)
std::pair<double, double> parseCoordinates(std::string input) {
    // finding comma in string
    size_t comma_pos = input.find(',');

    // if comma is not found
    if (comma_pos == std::string::npos) {
        std::cout << "Error: Invalid coordinate format: " << input << std::endl;
        exit(1);  
    }
    // use try catch to handle any unexpected errors when parsing string
    try {
        // assign x and y
        double x = std::stof(input.substr(0, comma_pos));
        double y = std::stof(input.substr(comma_pos + 1));
        // return x and y as pair
        return std::make_pair(x, y);
    } catch (const std::exception& e) {
        std::cerr << "Error parsing coordinates: " << e.what() << std::endl;
        exit(1);
    }
}


// parse area ranges (e.g., "1,1:5,5")
std::pair<std::pair<double, double>, std::pair<double, double> > parseArea(std::string input) {
    // find colon
    size_t colon_pos = input.find(':');
    // if colon not found
    if (colon_pos == std::string::npos) {
        std::cerr << "Error: Invalid area format: " << input << std::endl;
        exit(1);
    }

    // Parse start and end coordinates using parseCoordinates func
    std::pair<double, double> start = parseCoordinates(input.substr(0, colon_pos));
    std::pair<double, double> end = parseCoordinates(input.substr(colon_pos + 1));
    return std::make_pair(start, end);
}


// check we have a start and end coordinate but not conflicting ones
bool coordinate_check(bool start, bool end, bool start_area, bool end_area){
    if(start && end){
        if(start_area || end_area){
            std::cout << "Exact start and end with start or end area is not allowed!" << std::endl;
            return false;
        }
    }
    else if(start_area && end_area){
        if(start || end){
            std::cout << "Start and end area with exact start or end is not allowed!" << std::endl;
            return false;
        }
    }
    else if(start_area && end){
        if(start || end_area){
            std::cout << "Start area and exact end with exact start or end area is not allowed!" << std::endl;
            return false;
        }
    }
    else if(start && end_area){
        if(start_area || end){
            std::cout << "Exact start and end area with start area or exact end is not allowed!" << std::endl;
            return false;
        }
    }

    return true;

}

// check file exists for input helper
bool file_exist_check(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}


int main(int argc, char *argv[]) {
    // for passing to DEM
    std::vector<std::pair<double, double>> coords;

    // convert long_options into characters for switch case
    struct option long_options[] = {
    {"start", required_argument, nullptr, 's'},
    {"end", required_argument, nullptr, 'e'},
    {"start-area", required_argument, nullptr, 'a'},
    {"end-area", required_argument, nullptr, 'b'},
    {"input", required_argument, nullptr, 'i'},
    {"output", required_argument, nullptr, 'o'},
    {"iterations", required_argument, nullptr, 'n'},
    {"slope", required_argument, nullptr, 'p'},
    {"radius", required_argument, nullptr, 'r'},
    {"help", no_argument, nullptr, 'h'},
    {nullptr, 0, nullptr, 0}
    };

    // initialize vars
    int opt;

    std::string start, end, input_file, output_file;
    int iterations = -1;
    double slope_tolerance = -1;
    double radius = -1;
    std::string start_area, end_area;

    // for coordinate check func
    bool start_area_set = false;
    bool end_area_set = false;
    bool start_set = false;
    bool end_set = false;


    while ((opt = getopt_long(argc, argv, "s:e:a:b:i:o:n:p:h", long_options, nullptr)) != -1) {
        // switch cases
        switch (opt) {
            case 's': 
                start = optarg; 
                start_set = true; 
                break;
            case 'e': 
                end = optarg; 
                end_set = true;
                break;
            case 'a': 
                start_area = optarg;
                start_area_set = true; 
                break;
            case 'b': 
                end_area = optarg;
                end_area_set = true; 
                break;
            case 'i':  
                input_file = optarg; 
                break;
            case 'o': 
                output_file = optarg; 
                break;
            case 'n': 
                iterations = std::stoi(optarg);
                break;
            case 'p': 
                slope_tolerance = std::stof(optarg);
                break;
            case 'r':
                radius = std::stof(optarg);
                break;
            case 'h': 
                print_helper();
                return 0;
            default:
                std::cout << "Invalid Entry" << std::endl;
                print_helper();
                return 1;
        }
    }

    // check input file first to pass to DEM handler
    if(!file_exist_check(input_file)){
        std::cout << "Error: Input file does not exist or cannot be read" << std::endl;
        exit(1);
    }
    else{
        std::cout << "Input file found! -> " << input_file << std::endl;
    }

    // check output file
    if(!file_exist_check(output_file)){
        std::cout << "Error: Output file path is empty" << std::endl;
        exit(1);
    }
    else{
        std::cout << "Output file found! -> " << output_file  << std::endl;
    }
    // initialize DEM handler
    MEMPA::BUFFDEM demHandler(input_file, output_file);

    // check coordinates
    if(!coordinate_check(start_set,end_set,start_area_set,end_area_set)){
        std::cout << "Error: Check coordinate input" << std::endl;
        exit(1);
    }

    // if start coordinates given
    if (start_set) {
        // get pair of coordinates
        std::pair<double, double> start_coords = parseCoordinates(start); 
        // x
        double start_x = start_coords.first;   
        // y
        double start_y = start_coords.second;
        // print out for debugging
        std::cout << "Start X: " << start_x << ", Start Y: " << start_y << std::endl;
        
    }

    // Parse and validate end coordinates
    if (end_set) {
        // get pair of coordinates
        std::pair<double, double> end_coords = parseCoordinates(end);
        // x
        double end_x = end_coords.first;
        // y
        double end_y = end_coords.second;
        // print out for debugging
        std::cout << "End X: " << end_x << ", End Y: " << end_y << std::endl;
    }

    // Parse and validate start area
    if (start_area_set) {
        // get pair
        std::pair<std::pair<double, double>, std::pair<double, double> > area = parseArea(start_area);
    
        // individual coordinates
        double start_area_x1 = area.first.first;
        double start_area_y1 = area.first.second;
        double start_area_x2 = area.second.first;
        double start_area_y2 = area.second.second;
        // print out for debugging
        std::cout << "Start area: From (" << start_area_x1 << ", " << start_area_y1 << ") " << "to (" << start_area_x2 << ", " << start_area_y2 << ")" << std::endl;
    }

    // Parse and validate end area
    if (end_area_set) {
        // get pairs
        std::pair<std::pair<double, double>, std::pair<double, double> > area = parseArea(end_area);

        // individual coordinates
        double end_area_x1 = area.first.first;
        double end_area_y1 = area.first.second;
        double end_area_x2 = area.second.first;
        double end_area_y2 = area.second.second;
        // print out for debugging
        std::cout << "End area: From (" << end_area_x1 << ", " << end_area_y1 << ") " << "to (" << end_area_x2 << ", " << end_area_y2 << ")" << std::endl;
    }

    // check iterations is positive int and initialized
    if(iterations <= 0){
        std::cout << "Error: Iterations is less than or equal to 0 or not initialized" << std::endl;
        exit(1);
    }
    else{
        std::cout<< "Number of Iterations: " << iterations << std::endl; 
    }

    if(slope_tolerance == -1)
    {
        std::cout << "Error: No slope tolerance initialized" << std::endl;
        exit(1);
    }
    else{
        std::cout << "Slope Tolerance (in degrees): " << slope_tolerance << std::endl;
    }
    

    if(radius == -1){
        std::cout << "Error: No radius initialized" << std::endl;
        exit(1);
    }
    else{
        std::cout << "Visibility radius around the rover (in meters): " << radius << std::endl;
    }
    

    std::cout << "Program has successfully intialized all necessary inputs. Passing DEM file, coordinates, and radius to DEM Handler..." << std::endl;

}

