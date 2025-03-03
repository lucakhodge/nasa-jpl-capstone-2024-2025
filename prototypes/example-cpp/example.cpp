#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include <tuple>
#include <unistd.h>
using namespace std;
using json = nlohmann::json;

std::vector<std::tuple<int, int>> getPath(int x, int y, int X, int Y) {
  vector<tuple<int, int>> v;
  tuple<int, int> currentCoordinate = make_tuple(x, y);
  while (true) {
    int currentX = get<0>(currentCoordinate);
    int currentY = get<1>(currentCoordinate);
    v.push_back(currentCoordinate);
    if (currentX == X && currentY == Y) {
      return v;
    }
    int newX = currentX;
    int newY = currentY;
    if (X > currentX) {
      newX = currentX + 1;
    }
    if (X < currentX) {
      newX = currentX - 1;
    }
    if (Y > currentY) {
      newY = currentY + 1;
    }
    if (Y < currentY) {
      newY = currentY - 1;
    }
    currentCoordinate = make_tuple(newX, newY);
  }
}

string getJson(vector<tuple<int, int>> v) {
  json j;
  j["data"] = json::array();
  for (const auto &t : v) {
    j["data"].push_back({{"x", get<0>(t)}, {"y", get<1>(t)}});
  }
  return j.dump();
}

int main(int argc, char *argv[]) {
  int ch;
  int x = 0, y = 0, X = 0, Y = 0;
  string outFileName = "";
  string inFileName = "";
  try {
    while ((ch = getopt(argc, argv, "i:o:x:y:X:Y:")) != -1) {
      switch (ch) {
      case 'i':
        outFileName = optarg;
        break;
      case 'o':
        outFileName = optarg;
        break;
      case 'x':
        x = std::stoi(optarg);
        break;
      case 'y':
        y = std::stoi(optarg);
        break;
      case 'X':
        X = std::stoi(optarg);
        break;
      case 'Y':
        Y = std::stoi(optarg);
        break;
      case '?':
      default:
        cout << "Usage" << endl;
      }
    }
  } catch (std::exception const &e) {
    cout << "error : " << e.what() << endl;
  }

  if (outFileName.empty()) {
    cout << "No output file specified" << endl;
    return 1;
  }

  vector<tuple<int, int>> v = getPath(x, y, X, Y);

  // cout << "vec: " << endl;
  // for (auto i : v)
  //   cout << "(" << get<0>(i) << "," << get<1>(i) << ") ";
  //

  ofstream myfile;
  myfile.open(outFileName);
  myfile << getJson(v);
  myfile.close();
  // cout << getJson(v) << endl;

  return 0;
}
