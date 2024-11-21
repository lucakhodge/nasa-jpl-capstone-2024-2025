#include <iostream>
#include <getopt.h>
#include <string>
#include <fstream>

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