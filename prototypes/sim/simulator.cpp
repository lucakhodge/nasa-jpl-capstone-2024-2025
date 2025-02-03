/* Usage: 'g++ -std=c++17 && ./a.out' */

// import djikstra
// import DEM handler

#include <iostream>

class Simulator {
    public:
        std::string name;
        Simulator(std::string n) {
            name = n;
        }
};

int main(int argc, char** argv) {
    Simulator sim("NASA JPL Rover Sim");
    std::cout << sim.name << std::endl;
    return 0;
}