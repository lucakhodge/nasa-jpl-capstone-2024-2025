#include <iostream>
#include <getopt.h>
#include <string>
#include <fstream>
#include <regex>

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
              << "  --help           Print help message\n";
}

// split a string into x and y coordinates (for specific coordinates)
std::pair<float, float> parseCoordinates(std::string input) {
    // find comma seperating points
    size_t comma_pos = input.find(',');
    // if comma was found
    if (comma_pos != std::string::npos) {
        // update x
        int x = std::stoi(input.substr(0, comma_pos));
        // update y
        int y = std::stoi(input.substr(comma_pos + 1));
        // return coordinates as integers
        return {x, y};
    }
}

// parse area ranges (e.g., "1,1:5,5")
std::pair<std::pair<float, float>, std::pair<float, float>> parseArea(std::string input) {
    // find : seperating coordinates
    size_t colon_pos = input.find(':');
    // if : found
    if (colon_pos != std::string::npos) {
        // use coordinate functions
        std::pair start = parseCoordinates(input.substr(0, colon_pos));
        std::pair end = parseCoordinates(input.substr(colon_pos + 1));
        // return pairs of coordinates
        return {start, end};
    }
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
    // convert long_options into characters for switch case
    struct option long_options[] = {
        {"start", optional_argument, nullptr, 's'},
        {"end", optional_argument, nullptr, 'e'},
        {"start-area", optional_argument, nullptr, 'a'},
        {"end-area", optional_argument, nullptr, 'b'},
        {"input", required_argument, nullptr, 'i'},
        {"output", required_argument, nullptr, 'o'},
        {"iterations", required_argument, nullptr, 'n'},
        {"slope", required_argument, nullptr, 'p'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, 0}
    };

    // initialize vars
    int opt;

    std::string start, end, input_file, output_file, slope;
    int iterations = 0;
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
                slope = optarg; 
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

    // check coordinates
    if(!coordinate_check(start_set,end_set,start_area_set,end_area_set)){
        std::cout << "Error: Check coordinate input" << std::endl;
        exit(1);
    }
   
    // if start coordinates given
    if (start_set) {
        // get pair of coordinates
        std::pair<float, float> start_coords = parseCoordinates(start); 
        // x
        float start_x = start_coords.first;   
        // y
        float start_y = start_coords.second;
        // print out for debugging
        std::cout << "Start X: " << start_x << ", Start Y: " << start_y << std::endl;
        
    }

    // Parse and validate end coordinates
    if (end_set) {
        // get pair of coordinates
        std::pair<float, float> end_coords = parseCoordinates(end);
        // x
        float end_x = end_coords.first;
        // y
        float end_y = end_coords.second;
        // print out for debugging
        std::cout << "End X: " << end_x << ", End Y: " << end_y << std::endl;
    }

    // Parse and validate start area
    if (start_area_set) {
        // get pair
        std::pair<std::pair<float, float>, std::pair<float, float>> area = parseArea(start_area);
    
        // individual coordinates
        float start_area_x1 = area.first.first;
        float start_area_y1 = area.first.second;
        float start_area_x2 = area.second.first;
        float start_area_y2 = area.second.second;
        // print out for debugging
        std::cout << "Start area: From (" << start_area_x1 << ", " << start_area_y1 << ") " << "to (" << start_area_x2 << ", " << start_area_y2 << ")" << std::endl;
    }

    // Parse and validate end area
    if (end_area_set) {
        // get pairs
        std::pair<std::pair<float, float>, std::pair<float, float>> area = parseArea(end_area);

        // individual coordinates
        float end_area_x1 = area.first.first;
        float end_area_y1 = area.first.second;
        float end_area_x2 = area.second.first;
        float end_area_y2 = area.second.second;
        // print out for debugging
        std::cout << "End area: From (" << end_area_x1 << ", " << end_area_y1 << ") " << "to (" << end_area_x2 << ", " << end_area_y2 << ")" << std::endl;
    }

    // check input file
    if(!file_exist_check(input_file)){
        std::cout << "Error: Input file does not exist or cannot be read" << std::endl;
        exit(1);
    }

    // check output file
    if(!output_file.empty()){
        std::cout << "Error: Output file path is empty" << std::endl;
        exit(1);
    }

    // check iterations is positive int
    if(iterations <= 0){
        std::cout << "Error: Iterations is less than or equal to 0" << std::endl;
        exit(1);
    }

    // check slope is float
    // if(){
    //}
}