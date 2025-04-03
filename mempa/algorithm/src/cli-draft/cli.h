#ifndef CLICONFIG_H
#define CLICONFIG_H

#include <string>
#include <vector>
#include <utility>

class CLI {
public:
    std::string start, end, start_area, end_area, pixel_start_coord, pixel_end_coord;
    std::string input_file, output_file;
    int iterations;
    double slope_tolerance, radius;
    bool start_area_set, end_area_set, start_set, end_set, start_pixel, end_pixel;
    bool json_flag = false;
    std::vector<std::pair<double, double> > coordinates;
    std::vector<std::pair<int, int> > pixel_coordinates;

    CLI();

    bool coordinate_check();
    void processCoordinates();
    void parseArgs(int argc, char* argv[]);
    void print_helper();
    void validateInputs();
    void displayInputs();

private:

    std::pair<double, double> parseCoordinates(std::string input);
    std::pair<int, int> parsePixelCoordinates(std::string input);
    std::pair<std::pair<double, double>, std::pair<double, double> > parseArea(std::string input);
    bool file_exist_check(std::string filename);
    std::pair<double, double> getRandomPointInArea(double x1, double y1, double x2, double y2);

};
#endif
