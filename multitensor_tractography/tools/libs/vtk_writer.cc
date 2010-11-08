#include "vtk_writer.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>


bool VtkWriter::Write(
    const std::string& file_name,
    std::vector<vec_t> points,
    std::vector<std::vector<unsigned int> > lines,
    std::map<std::string, std::vector<std::vector<double> > > fields) {
  if (lines.size() == 0 || points.size() == 0) {
    std::cout << "No fibers existing." << std::endl;
    return true;
  }


  // Open file for writing.
  std::ofstream output(file_name.c_str());
  if(!output.is_open()) {
    std::cout << "Failed to open " << file_name << "." << std::endl;
    return true;
  }

  std::cout << "Writing to " << file_name << "." << std::endl;

  // Write header information
  output << "# vtk DataFile Version 3.0" << std::endl;
  output << "TractographyFibers" << std::endl;
  output << "ASCII" << std::endl;

  int num_fibers = lines.size();
  int num_points = points.size();

  // Write the points.
  output << "DATASET POLYDATA" << std::endl;
  output << "POINTS " << num_points << " float";
  int counter = 0;
  for (int i = 0; i < num_fibers; ++i) {
    for (size_t j = 0; j < lines[i].size(); ++j) {
      WritePoint(points[lines[i][j]], output, counter);
    }
  }
  output << std::endl;

  // Write the lines.
  counter = 0;
  output << std::endl << "LINES " << num_fibers << " "
         << num_fibers + num_points << std::endl;
  for (int i = 0; i < num_fibers; ++i) {
    int fiber_size = lines[i].size();
    output << fiber_size;
    for (int j = 0; j < fiber_size; ++j) {
      output << " " << counter++;
    }
    output << std::endl;
  }

  if (fields.size()) {
    output << std::endl << "POINT_DATA " << num_points << std::endl;
    output << "FIELD FieldData " << fields.size() << std::endl;
  }

  std::map<std::string, std::vector<std::vector<double> > >::const_iterator cit;
  for (cit = fields.begin(); cit != fields.end(); ++cit) {
    counter = 0;
    assert(cit->second.size() == static_cast<size_t>(num_points));
    int dim = cit->second[0].size();
    output << cit->first << " " << dim  << " " << num_points << " float"
           << std::endl;
    for (int j = 0; j < num_points; ++j) {
      for (int k = 0; k < dim; ++k) {
        output << cit->second[j][k];
        ++counter;
        if (!(counter % 9)) {
          output << std::endl;
        } else if (counter < num_points * dim) {
          output << " ";
        }
      }
    }
    if (counter % 9) {
      output << std::endl;
    }
  }

  output.close();

  return false;
}

void VtkWriter::WritePoint(const vec_t& point, std::ofstream& output,
                           int& counter) {
  // Always write nine floats onto one line.
  if (!(counter % 3)) {
    output << std::endl;
  } else {
    output << " ";
  }

  // Transform the point into the correct coordinate system.
  std::vector<double> p(4);
  p[0] = point._[2];
  p[1] = point._[1];
  p[2] = point._[0];

  output << p[2] << " " << p[1] << " " << p[0];
  ++counter;
}
