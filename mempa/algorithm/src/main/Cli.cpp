/*
Instructions for use:

** Make sure you are in correct directory where cli file is **
To Compile: g++ ./cli_main.cpp -o cli

** Fill in inputs and flags with correct ones **
To run: ./cli --flag INPUT

** Example **
./cli --start 44.43,55.55 --end 32.22,55.33 --input test.dem --output test.txt
--iterations 4 --slope 12.3 --radius 3.4

Required flags:
    2 of the following (1 being start and 1 being end):
        --start
        --end
        --start-pixel
        --end-pixel
        --start_area
        --end_area

    --input
    --output
    --iterations
    --slope
    --radius

** Use --help for printing out input formats with each flag **
*/

#include "Cli.hpp"
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <vector>

/**
 * @brief Construct a new CLI::CLI object and set all flags and variables to
 * false/-1
 *
 * @author Brock Hoos
 */
CLI::CLI()
    : iterations(-1), slope_tolerance(-1), radius(-1), start_area_set(false),
      end_area_set(false), start_set(false), end_set(false), start_pixel(false),
      end_pixel(false), coordinates() {}

/**
 * @brief Takes in string input form user, and parses coordinates for delimiter
 * ',' and makes sure coordinates are in correct format. (For geographical
 * coordinates of double type)
 *
 * @param input
 * @return std::pair<double, double>
 *
 * @author Brock Hoos
 */
std::pair<double, double> CLI::parseCoordinates(std::string input) {
  size_t comma_pos = input.find(',');
  if (comma_pos == std::string::npos) {
    throw std::invalid_argument("Invalid coordinate format: " + input);
  }
  double x = std::stod(input.substr(0, comma_pos));
  double y = std::stod(input.substr(comma_pos + 1));
  return std::make_pair(x, y);
}

/**
 * @brief Takes in string input form user, and parses coordinates for delimiter
 * ',' and makes sure coordinates are in correct format. (For pixel coordinates
 * of int type)
 *
 * @param input
 * @return std::pair<int, int>
 *
 * @author Brock Hoos
 */
std::pair<int, int> CLI::parsePixelCoordinates(std::string input) {
  size_t comma_pos = input.find(',');
  if (comma_pos == std::string::npos) {
    throw std::invalid_argument("Invalid coordinate format: " + input);
  }
  int x = std::stod(input.substr(0, comma_pos));
  int y = std::stod(input.substr(comma_pos + 1));
  return std::make_pair(x, y);
}

/**
 * @brief Takes in 2 coordinates (area) and uses ',' as delimiter to correctly
 * parse area and check format
 *
 * @param input
 * @return std::pair<std::pair<double, double>, std::pair<double, double> >
 *
 * @author Brock Hoos
 */
std::pair<std::pair<double, double>, std::pair<double, double>>
CLI::parseArea(std::string input) {
  size_t colon_pos = input.find(':');
  if (colon_pos == std::string::npos) {
    throw std::invalid_argument("Invalid area format: " + input);
  }
  std::pair<double, double> start =
      parseCoordinates(input.substr(0, colon_pos));
  std::pair<double, double> end = parseCoordinates(input.substr(colon_pos + 1));
  return std::make_pair(start, end);
}

/**
 * @brief Makes sure that CLI received only 1 start coordinate and 1 end
 * coordinate.
 *
 * @return true
 * @return false
 *
 * @author Brock Hoos
 */
bool CLI::coordinate_check() {
  int start_count = start_set + start_area_set + start_pixel;
  int end_count = end_set + end_area_set + end_pixel;

  // have exactly one start and one end type
  if (start_count != 1 || end_count != 1) {
    return false;
  }

  // pixel coordinates are not mixed with others
  if ((start_pixel || end_pixel) &&
      (start_set || start_area_set || end_set || end_area_set)) {
    return false;
  }

  return true;
}

/**
 * @brief Checks to make sure input and output files exist
 *
 * @param filename
 * @return true
 * @return false
 *
 * @author Brock Hoos
 */
bool CLI::file_exist_check(std::string filename) {
  std::ifstream file(filename);
  return file.good();
}

/**
 * @brief Using switch statements to parse inputs given through POSIX flags from
 * user and set internal flags for which type of coordinates received
 *
 * @param argc
 * @param argv
 *
 * @author Brock Hoos
 */
void CLI::parseArgs(int argc, char *argv[]) {
  struct option long_options[] = {
      {"start", required_argument, nullptr, 's'},
      {"end", required_argument, nullptr, 'e'},
      {"start-area", required_argument, nullptr, 'a'},
      {"end-area", required_argument, nullptr, 'b'},
      {"start-pixel", required_argument, nullptr, 'x'},
      {"end-pixel", required_argument, nullptr, 'y'},
      {"input", required_argument, nullptr, 'i'},
      {"output", required_argument, nullptr, 'o'},
      {"iterations", required_argument, nullptr, 'n'},
      {"slope", required_argument, nullptr, 'p'},
      {"radius", required_argument, nullptr, 'r'},
      {"json", no_argument, nullptr, 'j'},
      {"help", no_argument, nullptr, 'h'},
      {nullptr, 0, nullptr, 0}};

  int opt;
  while ((opt = getopt_long(argc, argv, "s:e:a:b:i:o:n:p:h", long_options,
                            nullptr)) != -1) {
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
    case 'x':
      pixel_start_coord = optarg;
      start_pixel = true;
      break;
    case 'y':
      pixel_end_coord = optarg;
      end_pixel = true;
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
      slope_tolerance = std::stod(optarg);
      break;
    case 'r':
      radius = std::stod(optarg);
      break;
    case 'j':
      json_flag = true;
      break;
    case 'h':
      print_helper();
      return;
    default:
      std::cerr << "Invalid option" << std::endl;
      print_helper();
      return;
    }
  }
}

/**
 * @brief Print out helper of flags for each type of input
 *
 * @author Brock Hoos
 */
void CLI::print_helper() {
  std::cout << "Options:\n"
            << "  --start          Start coordinates (e.g., 1,1 or 'random')\n"
            << "  --end            End coordinates (e.g., 10,10)\n"
            << "  --start-area     Start area range (e.g., 1,1:5,5)\n"
            << "  --end-area       End area range (e.g., 1,1:5,5)\n"
            << "  --start-pixel    End area range (e.g., 1,1:5,5)\n"
            << "  --end-pixel      End area range (e.g., 1,1:5,5)\n"
            << "  --input          Input DEM file\n"
            << "  --output         Output results file\n"
            << "  --iterations     Number of iterations\n"
            << "  --slope          Slope tolerances (e.g., 10,20,30)\n"
            << "  --radius         Visibility Radius of Rover (in meters)\n"
            << "  --json           Print output into JSON format\n"
            << "  --help           Print help message\n";
}

/**
 * @brief Helper for cli_main.cpp to validate all inputs in single method.
 * (coordinate validation is done in coordinate_check())
 *
 * @author Brock Hoos
 *
 */
void CLI::validateInputs() {
  // check input file
  if (!file_exist_check(input_file)) {
    std::cerr << "Error: Input file does not exist or cannot be read"
              << std::endl;
    exit(1);
  }
  // removed needing an output file as this is no long required.  We Might just
  // want to completly remove this functionality.
  //  check output file
  //  if (!file_exist_check(output_file)) {
  //      std::cerr << "Error: Output file does not exist or cannot be read" <<
  //      std::endl; exit(1);
  //  }
  //  check iterations
  if (iterations <= 0) {
    std::cerr << "Error: Iterations must be initialized and greater than 0"
              << std::endl;
    exit(1);
  }
  // check slope
  if (slope_tolerance <= 0) {
    std::cerr << "Error: Slope tolerance must be initialized and greater than 0"
              << std::endl;
    exit(1);
  }
  // check radius
  if (radius <= 0) {
    std::cerr
        << "Error: Visibility radius must be initialized and greater than 0"
        << std::endl;
    exit(1);
  }
}

/**
 * @brief Display inputs received and initialized from user
 *
 * @author Brock Hoos
 *
 */
void CLI::displayInputs() {
  std::cout << "Start: " << start << "\nEnd: " << end
            << "\nStart Area: " << start_area << "\nEnd Area: " << end_area
            << "\nStart (Pixel): " << pixel_start_coord
            << "\nEnd (Pixel): " << pixel_end_coord << "\n";
  std::cout << "Input File: " << input_file << "\nOutput File: " << output_file
            << "\nIterations: " << iterations << "\n";
  std::cout << "Slope Tolerance: " << slope_tolerance << "\nRadius: " << radius
            << std::endl;
}

/**
 * @brief Helper for DEM Handler. Get random point in area of coordinates to
 * pass one single point to DEM Handler
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @return std::pair<double, double>
 *
 * @author Brock Hoos
 */
std::pair<double, double> CLI::getRandomPointInArea(double x1, double y1,
                                                    double x2, double y2) {
  double random_x = x1 + static_cast<double>(rand()) / RAND_MAX * (x2 - x1);
  double random_y = y1 + static_cast<double>(rand()) / RAND_MAX * (y2 - y1);

  return std::make_pair(random_x, random_y);
}

/**
 * @brief Given what type of flag was used, correctly initialize coordinates
 * vector with correct inputs Start coordinate first, then end coordinate in
 * vector
 *
 * @author Brock Hoos
 *
 */
void CLI::processCoordinates() {

  if (start_set) {
    std::pair<double, double> start_coords = parseCoordinates(start);
    coordinates.push_back(start_coords);
    std::cout << "Start Coordinates: (" << start_coords.first << ", "
              << start_coords.second << ")" << std::endl;
  }

  if (start_area_set) {
    std::pair<std::pair<double, double>, std::pair<double, double>>
        start_area_coords = parseArea(start_area);
    std::cout << "Start Area: From (" << start_area_coords.first.first << ", "
              << start_area_coords.first.second << ") to ("
              << start_area_coords.second.first << ", "
              << start_area_coords.second.second << ")" << std::endl;
    std::pair<double, double> random_start_point = getRandomPointInArea(
        start_area_coords.first.first, start_area_coords.first.second,
        start_area_coords.second.first, start_area_coords.second.second);
    coordinates.push_back(random_start_point);
    std::cout << "Random Start Point: (" << random_start_point.first << ", "
              << random_start_point.second << ")" << std::endl;
  }

  if (start_pixel) {
    std::pair<int, int> start_coords = parsePixelCoordinates(pixel_start_coord);
    pixel_coordinates.push_back(start_coords);
    std::cout << "Start Coordinates (Pixel): (" << start_coords.first << ", "
              << start_coords.second << ")" << std::endl;
  }

  if (end_set) {
    std::pair<double, double> end_coords = parseCoordinates(end);
    coordinates.push_back(end_coords);
    std::cout << "End Coordinates: (" << end_coords.first << ", "
              << end_coords.second << ")" << std::endl;
  }

  if (end_area_set) {
    std::pair<std::pair<double, double>, std::pair<double, double>>
        end_area_coords = parseArea(end_area);
    std::cout << "End Area: From (" << end_area_coords.first.first << ", "
              << end_area_coords.first.second << ") to ("
              << end_area_coords.second.first << ", "
              << end_area_coords.second.second << ")" << std::endl;
    std::pair<double, double> random_end_point = getRandomPointInArea(
        end_area_coords.first.first, end_area_coords.first.second,
        end_area_coords.second.first, end_area_coords.second.second);
    coordinates.push_back(random_end_point);
    std::cout << "Random End Point: (" << random_end_point.first << ", "
              << random_end_point.second << ")" << std::endl;
  }

  if (end_pixel) {
    std::pair<int, int> end_coords = parsePixelCoordinates(pixel_end_coord);
    pixel_coordinates.push_back(end_coords);
    std::cout << "End Coordinates (Pixel): (" << end_coords.first << ", "
              << end_coords.second << ")" << std::endl;
  }
}
