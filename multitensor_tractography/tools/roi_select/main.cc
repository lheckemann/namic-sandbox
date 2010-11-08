#include <cassert>
#include <cstdlib>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>
#include "teem/nrrd.h"
#include "libs/vtk_reader.h"
#include "libs/vtk_writer.h"
#include "libs/signal_data.h"


int GetLabel(const Nrrd* label_nrrd, const NrrdData* signal, vec_t pos);

int main(int argc, char **argv) {
  // Default parameters.
  const char *fiber_file = NULL;
  const char *roi_file = NULL;
  const char *dwi_file = NULL;

  // Labels.
  std::set<int> pos_labels;
  std::set<int> neg_labels;

  bool and_op = true;

  // Parse command line arguments.
  std::string help_string =
      std::string("Usage:\n") + argv[0] + "\n" +
      "  --fiber_file FILE\n  --roi_file FILE\n  --dwi_file FILE\n"
      "  --add_labels INT(,INT)*\n  [--sub_labels INT(,INT)*]\n"
      "  [--operation (and|or)]\n";
      
  bool error = false;
  int i = 1;
  if (argc % 2) {
    while (i + 1 < argc) {
      if (!std::string(argv[i]).compare("--fiber_file")) {
        fiber_file = argv[++i];
      } else if (!std::string(argv[i]).compare("--roi_file")) {
        roi_file = argv[++i];
      } else if (!std::string(argv[i]).compare("--dwi_file")) {
        dwi_file = argv[++i];
      } else if (!std::string(argv[i]).compare("--add_labels")) {
        std::string str(argv[++i]);
        size_t found = str.find_first_of(",");
        pos_labels.insert(atoi(str.c_str()));
        while (found != std::string::npos) {
          pos_labels.insert(atoi(str.substr(found + 1).c_str()));
          found = str.find_first_of(",", found + 1);
        }
      } else if (!std::string(argv[i]).compare("--sub_labels")) {
        std::string str(argv[++i]);
        size_t found = str.find_first_of(",");
        neg_labels.insert(atoi(str.c_str()));
        while (found != std::string::npos) {
          neg_labels.insert(atoi(str.substr(found + 1).c_str()));
          found = str.find_first_of(",", found + 1);
        }
      } else if (!std::string(argv[i]).compare("--operation")) {
        if (!std::string(argv[++i]).compare("or")) {
          and_op = false;
        } else if (std::string(argv[++i]).compare("and")) {
          error = true;
        }
      } else {
        error = true;
        break;
      }
      ++i;
    }
  } else {
    error = true;
  }

  // Handle erroneous input.
  if (!fiber_file || !roi_file || !dwi_file || !pos_labels.size() || error) {
    std::cout << "Error!" << std::endl << std::endl;
    std::cout << help_string << std::endl;
    return 1;
  }

  Nrrd *roi_nrrd = nrrdNew();
  char *err;
  if (nrrdLoad(roi_nrrd, roi_file, NULL)) {
    err = biffGetDone(NRRD);
    std::cout << "Trouble reading " << roi_file << ": " << err << std::endl;
    delete err;
    nrrdNuke(roi_nrrd);
    return 1;
  }

  const double sigma = 1.0;
  NrrdData signal(sigma);

  if (signal.LoadSignal(dwi_file)) {
    std::cout << "Signal could not be loaded" << std::endl;
    nrrdNuke(roi_nrrd);
    return 1;
  }
  //std::cout << signal.i2r() << std::endl;
  //std::cout << signal.r2i() << std::endl;

  VtkReader reader;
  reader.Read(fiber_file);
  //reader.Print();
  const std::vector<vec_t>& points = reader.points();
  const std::vector<std::vector<unsigned int> >& lines = reader.lines();
  const std::map<std::string, std::vector<std::vector<double> > >& fields =
      reader.fields();

  std::map<int, int> indices;
  std::vector<vec_t> points_new;
  std::vector<std::vector<unsigned int> > lines_new;
  std::map<std::string, std::vector<std::vector<double> > > fields_new;


  int count = 0;

  bool keep;
  std::map<std::string, std::vector<std::vector<double> > >::const_iterator cit;
  std::pair<std::map<int, int>::iterator, bool> res;
  for (size_t i = 0; i < lines.size(); i++) {
    keep = true;
    std::set<int> found_labels;

    for (size_t j = 0; j < lines[i].size(); j++) {
      int label = GetLabel(roi_nrrd, &signal, points[lines[i][j]]);

      // Test for the subracting labels.
      std::set<int>::const_iterator cit;
      for (cit = neg_labels.begin(); cit != neg_labels.end(); ++cit) {
        if (*cit == label) {
          keep = false;
          break;
        }
      }

      if (!keep) {
        break;
      }

      found_labels.insert(label);
    }

    if (keep) {
      // Test for adding labels.
      std::set<int>::const_iterator cit;

      //for (cit = found_labels.begin(); cit != found_labels.end(); ++cit) {
      //  std::cout << *cit << " ";
      //}
      //std::cout << std::endl;

      if (and_op) {
        for (cit = pos_labels.begin(); cit != pos_labels.end(); ++cit) {
          if (found_labels.find(*cit) == found_labels.end()) {
            keep = false;
            break;
          }
        }
      } else {
        keep = false;
        for (cit = pos_labels.begin(); cit != pos_labels.end(); ++cit) {
          if (found_labels.find(*cit) != found_labels.end()) {
            keep = true;
            break;
          }
        }
      }
    }

    if (keep) {
      // Keep the line.
      std::vector<unsigned int> line(lines[i].size());
      for (size_t j = 0; j < lines[i].size(); j++) {
        int index = lines[i][j];
        int new_index;
        res = indices.insert(std::pair<int, int>(index, count));

        if (!res.second) {
          new_index = res.first->second;
        } else {
          new_index = count;

          // Copy all the data.
          points_new.push_back(points[index]);
          for (cit = fields.begin(); cit != fields.end(); ++cit) {
            fields_new[cit->first].push_back(fields.find(cit->first)->second[index]);
          }

          ++count;
        }
        line[j] = new_index;
      }
      lines_new.push_back(line);
    } else {
      std::cout << "throw away" << std::endl;
    }
  }

  std::string str(fiber_file);
  std::stringstream ss;
  ss << str.substr(0, str.find_last_of(".")) << "_reduced" << ".vtk";

  VtkWriter writer;
  writer.Write(ss.str(), points_new, lines_new, fields_new);

  return 0;
}

int GetLabel(const Nrrd* label_nrrd, const NrrdData* signal, vec_t pos) {
  std::vector<double> p(4);
  p[0] = pos._[0];
  p[1] = pos._[1];
  p[2] = pos._[2];
  p[3] = 1.0;

  std::vector<double> p_new(4);
  gmm::mult(signal->r2i(), p, p_new);
  //output << p_new[0] << " " << p_new[1] << " " << p_new[2];

  pos._[0] = p_new[2];
  pos._[1] = p_new[1];
  pos._[2] = p_new[0];

  // Go through the volume.
  int nx = label_nrrd->axis[2].size;
  int ny = label_nrrd->axis[1].size;
  int nz = label_nrrd->axis[0].size;

  int data_type = label_nrrd->type;

  int x = static_cast<int>(floor(pos._[0]));
  int y = static_cast<int>(floor(pos._[1]));
  int z = static_cast<int>(floor(pos._[2]));
  if (x < 0) {
    x = 0;
  }
  if (y < 0) {
    y = 0;
  }
  if (z < 0) {
    z = 0;
  }
  if (x >= nx) {
    x = nx - 1;
  }
  if (y >= ny) {
    y = ny - 1;
  }
  if (z >= nz) {
    z = nz - 1;
  }

  //std::cout << "pos: " << x << " " << y << " " << z << std::endl;

  int index = ny * nz * x + nz * y + z;
  int value = 0;
  switch (data_type) {
  case 2:
    value = static_cast<unsigned char *>(label_nrrd->data)[index];
    break;
  case 3:
    value = static_cast<short *>(label_nrrd->data)[index];
    break;
  case 5:
    value = static_cast<int *>(label_nrrd->data)[index];
    break;
  default:
    std::cout << "Unsupported data type for label nrrd file!" << std::endl;
    assert(false);
  }

  return value;
}
