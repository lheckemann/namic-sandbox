#include "vtk_reader.h"
#include <cassert>
#include <fstream>
#include <iostream>


// Assumes that the file is in the correct format.
bool VtkReader::Read(const std::string& file_name) {
  // Open file for reading.
  std::ifstream input(file_name.c_str());
  if(!input.is_open()) {
    std::cout << "Failed to open " << file_name << "." << std::endl;
    return true;
  }

  // Parse file.
  int num_points = 0;
  int num_lines = 0;
  int state_dim = 0;
  std::string s;
  while (!input.eof()) {
    while (input.peek() == '#') {
      getline(input, s);
    }

    input >> s;

    // Get points.
    if (!s.compare("DATASET")) {
      input >> s;
      assert(!s.compare("POLYDATA"));
      input >> s;
      assert(!s.compare("POINTS"));

      input >> num_points;
      _points.resize(num_points);

      input >> s;
      assert(!s.compare("float"));

      double x, y, z;
      for (int i = 0; i < num_points; ++i) {
        input >> x;
        input >> y;
        input >> z;

        _points[i] = make_vec(x, y, z);
      }
    } else if (!s.compare("LINES")) {
      input >> num_lines;
      _lines.resize(num_lines);

      int tmp;
      input >> tmp;
      assert(num_points + num_lines == tmp);

      int length;
      for (int i = 0; i < num_lines; ++i) {
        input >> length;
        _lines[i].resize(length);

        for (int j = 0; j < length; ++j) {
          input >> _lines[i][j];
        }
      }
//    } else if (!s.compare("POINT_DATA")) {
//      int tmp;
//      input >> tmp;
//      assert(tmp == num_points);
//      input >> s;
//      assert(!s.compare("FIELD"));
//      input >> s;
//      assert(!s.compare("FieldData"));
    } else if (!s.compare("state")) {
      input >> state_dim;
      int tmp;
      input >> tmp;
      assert(tmp == num_points);
      input >> s;
      assert(!s.compare("float"));

      _states.resize(num_points);
      for (int i = 0; i < num_points; ++i) {
        _states[i].resize(state_dim);

        for (int j = 0; j < state_dim; ++j) {
          input >> _states[i][j];
        }
      }
    }

    getline(input, s);
  }

  assert(_points.size());
  assert(_lines.size());
  assert(_states.size() == static_cast<unsigned int>(num_points));

  input.close();

  return false;
}

void VtkReader::Print() const {
  std::cout << "Points (" << _points.size() << "):" << std::endl;
  for (size_t i = 0; i < _points.size(); ++i) {
    std::cout << _points[i]._[0] << " " << _points[i]._[1] << " "
              << _points[i]._[2] << std::endl;
  }

  std::cout << "Lines (" << _lines.size() << "):" << std::endl;
  for (size_t i = 0; i < _lines.size(); ++i) {
    std::cout << _lines[i].size() << ": ";
    for (size_t j = 0; j < _lines[i].size(); ++j) {
      std::cout << " " << _lines[i][j];
    }
    std::cout << std::endl;
  }

  std::cout << "State (" << _states.size() << "):" << std::endl;
  for (size_t i = 0; i < _states.size(); ++i) {
    std::cout << _states[i][0];
    for (size_t j = 1; j < _states[i].size(); ++j) {
      std::cout << " " << _states[i][j];
    }
    std::cout << std::endl;
  }
}
