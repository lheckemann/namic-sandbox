#include "vtk_writer.h"
#include <fstream>
#include <iostream>
#include <gmm/gmm.h>
#include "signal_data.h"


bool VtkWriter::Write(const std::string& file_name,
                      const std::vector<Tractography::Fiber>& fibers) {
  if (fibers.size() == 0) {
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

  int num_fibers = fibers.size();
  int num_points = 0;
  for (int i = 0; i < num_fibers; ++i) {
    num_points += fibers[i].position.size();
  }

  // Write the points.
  output << "DATASET POLYDATA" << std::endl;
  output << "POINTS " << num_points << " float" << std::endl;
  int counter = 0;
  for (int i = 0; i < num_fibers; ++i) {
    int fiber_size = fibers[i].position.size();
    for (int j = 0; j < fiber_size; ++j) {
      // Transform the point into the correct coordinate system.
      std::vector<double> p(4);
      p[0] = fibers[i].position[j]._[2];
      p[1] = fibers[i].position[j]._[1];
      p[2] = fibers[i].position[j]._[0];

      if (_transform_position) {
        p[3] = 1.0;
        std::vector<double> p_new(4);
        gmm::mult(_signal_data->i2r(), p, p_new);

        output << p_new[0] << " " << p_new[1] << " " << p_new[2];
      } else {
        output << p[2] << " " << p[1] << " " << p[0];
      }
      ++counter;

      // Always write nine floats onto one line.
      if (!(counter % 3)) {
        output << std::endl;
      } else if (counter < num_points) {
        output << " ";
      }
    }
  }
  if (counter % 3) {
    output << std::endl;
  }

  // Write the lines.
  counter = 0;
  output << std::endl << "LINES " << num_fibers << " "
         << num_fibers + num_points << std::endl;
  for (int i = 0; i < num_fibers; ++i) {
    int fiber_size = fibers[i].position.size();
    output << fiber_size;
    for (int j = 0; j < fiber_size; ++j) {
      output << " " << counter++;
    }
    output << std::endl;
  }

  output << std::endl << "POINT_DATA " << num_points << std::endl;
  int fields = 1; // 1 for the norm.
  bool write_fa = fibers[0].fa.size();
  bool write_state = fibers[0].state.size();
  bool write_cov = fibers[0].covariance.size();
  if (write_fa) {
    ++fields;
  }
  if (write_state) {
    ++fields;
  }
  if (write_cov) {
    ++fields;
  }
  output << "FIELD FieldData " << fields << std::endl;

  // Write norm.
  counter = 0;
  output << "norm 1 " << num_points << " float" << std::endl;
  for (int i = 0; i < num_fibers; ++i) {
    int fiber_size = fibers[i].position.size();
    for (int j = 0; j < fiber_size; ++j) {
      output << fibers[i].norm[j];
      ++counter;
      if (!(counter % 9)) {
        output << endl;
      } else if (counter < num_points) {
        output << " ";
      }
    }
  }
  if (counter % 9) {
    output << std::endl;
  }

  // Write additional information.
  if (write_fa) {
    output << "FA 1 " << num_points << " float" << std::endl;
    counter = 0;
    for (int i = 0; i < num_fibers; ++i) {
      int fiber_size = fibers[i].position.size();
      for (int j = 0; j < fiber_size; ++j) {
        output << fibers[i].fa[j] * 255;
        ++counter;
        if (!(counter % 9)) {
          output << endl;
        } else if (counter < num_points) {
          output << " ";
        }
      }
    }
    if (counter % 9) {
      output << std::endl;
    }
  }
  if (write_state) {
    int state_dim = fibers[0].state[0].size();
    output << "state " << state_dim << " " << num_points << " float"
           << std::endl;
    counter = 0;
    for (int i = 0; i < num_fibers; ++i) {
      int fiber_size = fibers[i].position.size();
      for (int j = 0; j < fiber_size; ++j) {
        for (int k = 0; k < state_dim; ++k) {
          output << fibers[i].state[j][k];
          ++counter;
          if (!(counter % 9)) {
            output << endl;
          } else if (counter < num_points * state_dim) {
            output << " ";
          }
        }
      }
    }
    if (counter % 9) {
      output << std::endl;
    }
  }
  if (write_cov) {
    int state_dim = fibers[0].state[0].size();
    output << "covariance " << state_dim * (state_dim + 1) / 2 << " "
           << num_points << " float" << std::endl;
    counter = 0;
    for (int i = 0; i < num_fibers; ++i) {
      int fiber_size = fibers[i].position.size();
      for (int j = 0; j < fiber_size; ++j) {
        for (int a = 0; a < state_dim; ++a) {
          for (int b = a; b < state_dim; ++b) {
            output << fibers[i].covariance[j](a, b);
            ++counter;
            if (!(counter % 9)) {
              output << endl;
            } else if (counter < num_points * state_dim * (state_dim + 1) / 2) {
              output << " ";
            }
          }
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
