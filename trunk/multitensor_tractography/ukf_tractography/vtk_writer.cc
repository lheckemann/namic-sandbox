#include "vtk_writer.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <gmm/gmm.h>
#include "signal_data.h"


bool VtkWriter::Write(const std::string& file_name,
                      const std::vector<Tractography::Fiber>& fibers,
                      bool write_state, bool store_glyphs) {
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
  if (store_glyphs) {
    assert(fibers[0].state.size());
    std::stringstream ss;
    ss << file_name.substr(0, file_name.find_last_of(".")) << "_glyphs"
       << ".vtk";
    if (WriteGlyphs(ss.str(), fibers)) {
      return true;
    }
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
  output << "POINTS " << num_points << " float";
  int counter = 0;
  for (int i = 0; i < num_fibers; ++i) {
    int fiber_size = fibers[i].position.size();
    for (int j = 0; j < fiber_size; ++j) {
      WritePoint(fibers[i].position[j], output, counter);
    }
  }
  output << std::endl;

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

bool VtkWriter::WriteGlyphs(const std::string& file_name,
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

  std::cout << "Writing glyphs to " << file_name << "." << std::endl;

  // Write header information
  output << "# vtk DataFile Version 3.0" << std::endl;
  output << "TractographyGlyphs" << std::endl;
  output << "ASCII" << std::endl;

  int num_fibers = fibers.size();
  int num_points = 0;
  for (int i = 0; i < num_fibers; ++i) {
    num_points += fibers[i].position.size();
  }

  int num_tensors = fibers[0].state[0].size() / 5;

  const double scale = 0.5 * _scale_glyphs;

  // Write the points.
  output << "DATASET POLYDATA" << std::endl;
  output << "POINTS " << num_points * 2 * num_tensors << " float";
  int counter = 0;
  for (int i = 0; i < num_fibers; ++i) {
    int fiber_size = fibers[i].position.size();
    for (int j = 0; j < fiber_size; ++j) {
      vec_t point = fibers[i].position[j];
      const State& state = fibers[i].state[j];

      // Get the directions.
      vec_t m1, m2, m3;
      if (state.size() == 5) {
        m1 = make_vec(state[2], state[1], state[0]);
        m1 = state[3] / 100.0 * m1;
      } else if (state.size() == 6) {
        m1 = rotation_main_dir(state[0], state[1], state[2]);
        double tmp = m1._[0];
        m1._[0] = m1._[2];
        m1._[2] = tmp;
        m1 = state[3] / 100.0 * m1;
      } else if (state.size() == 10) {
        m1 = make_vec(state[2], state[1], state[0]);
        m2 = make_vec(state[7], state[6], state[5]);
        m1 = state[3] / 100.0 * m1;
        m2 = state[8] / 100.0 * m2;
      } else if (state.size() == 12) {
        m1 = rotation_main_dir(state[0], state[1], state[2]);
        m2 = rotation_main_dir(state[6], state[7], state[8]);
        double tmp = m1._[0];
        m1._[0] = m1._[2];
        m1._[2] = tmp;
        tmp = m2._[0];
        m2._[0] = m2._[2];
        m2._[2] = tmp;
        m1 = state[3] / 100.0 * m1;
        m2 = state[9] / 100.0 * m2;
      } else if (state.size() == 15) {
        m1 = make_vec(state[2], state[1], state[0]);
        m2 = make_vec(state[7], state[6], state[5]);
        m3 = make_vec(state[12], state[11], state[10]);
        m1 = state[3] / 100.0 * m1;
        m2 = state[8] / 100.0 * m2;
        m3 = state[13] / 100.0 * m3;
      } else if (state.size() == 18) {
        m1 = rotation_main_dir(state[0], state[1], state[2]);
        m2 = rotation_main_dir(state[6], state[7], state[8]);
        m3 = rotation_main_dir(state[12], state[13], state[14]);
        double tmp = m1._[0];
        m1._[0] = m1._[2];
        m1._[2] = tmp;
        tmp = m2._[0];
        m2._[0] = m2._[2];
        m2._[2] = tmp;
        tmp = m3._[0];
        m3._[0] = m3._[2];
        m3._[2] = tmp;
        m1 = state[3] / 100.0 * m1;
        m2 = state[9] / 100.0 * m2;
        m3 = state[15] / 100.0 * m3;
      }

      // Calculate the points. The glyphs are represented as two-point lines.
      vec_t pos1, pos2;
      if (num_tensors == 1) {
        pos1 = point - scale * m1;
        pos2 = point + scale * m1;
        WritePoint(pos1, output, counter);
        WritePoint(pos2, output, counter);
      } else if (num_tensors == 2) {
        pos1 = point - scale * m1;
        pos2 = point + scale * m1;
        WritePoint(pos1, output, counter);
        WritePoint(pos2, output, counter);

        pos1 = point - scale * m2;
        pos2 = point + scale * m2;
        WritePoint(pos1, output, counter);
        WritePoint(pos2, output, counter);
      } else if (num_tensors == 3) {
        pos1 = point - scale * m1;
        pos2 = point + scale * m1;
        WritePoint(pos1, output, counter);
        WritePoint(pos2, output, counter);

        pos1 = point - scale * m2;
        pos2 = point + scale * m2;
        WritePoint(pos1, output, counter);
        WritePoint(pos2, output, counter);

        pos1 = point - scale * m3;
        pos2 = point + scale * m3;
        WritePoint(pos1, output, counter);
        WritePoint(pos2, output, counter);
      }
    }
  }
  output << std::endl;

  // Write the lines.
  output << std::endl << "LINES " << num_points * num_tensors << " "
         << num_points * num_tensors * 3 << std::endl;
  for (int i = 0; i < num_points * num_tensors; ++i) {
    output << "2 " << i * 2 << " " << i * 2 + 1 << std::endl;
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

  if (_transform_position) {
    p[3] = 1.0;
    std::vector<double> p_new(4);
    gmm::mult(_signal_data->i2r(), p, p_new);

    output << p_new[0] << " " << p_new[1] << " " << p_new[2];
  } else {
    output << p[2] << " " << p[1] << " " << p[0];
  }
  ++counter;
}
