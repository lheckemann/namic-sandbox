#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include "teem/nrrd.h"


double RandomUniform();
double RandomNormal();

int main(int argc, char** argv) {
  std::string help_string =
      std::string("Usage:\n") + argv[0] + " --dwi_file FILE --sigma DOUBLE";

  if (argc != 5) {
    std::cout << "Error!" << std::endl << std::endl;
    std::cout << help_string << std::endl;
    return 1;
  }

  const char *dwi_file = NULL;
  double sigma = -1.0;

  // Parse command line arguments.
  bool error = false;
  int i = 1;
  if (argc % 2) {
    while (i + 1 < argc) {
      if (!std::string(argv[i]).compare("--dwi_file")) {
        dwi_file = argv[++i];
      } else if (!std::string(argv[i]).compare("--sigma")) {
        sigma = atof(argv[++i]);
      } else {
        error = true;
        break;
      }
      ++i;
    }
  }

  if (!dwi_file || sigma < 0.0 || error) {
    std::cout << "Error!" << std::endl << std::endl;
    std::cout << help_string << std::endl;
    return 1;
  }

  // Sigma for small noise would be around 3 and around 2 for much noise.
  srand(time(NULL));
  assert(sigma > 0);

  std::string input_file_name(dwi_file);
  std::stringstream ss;
  ss << input_file_name.substr(0, input_file_name.find_last_of("."))
     << "_noise_sigma" << sigma << ".nrrd";
  std::string output_file_name(ss.str());

  Nrrd *nrrd = nrrdNew();

  char *err;
  if (nrrdLoad(nrrd, input_file_name.c_str(), NULL)) {
    err = biffGetDone(NRRD);
    std::cout << "Trouble reading " << input_file_name << ": " << err
              << std::endl;
    delete err;
    nrrdNuke(nrrd);
    return 1;
  }

  // Assure that the data type is 'float'.
  assert(nrrd->type == 9);
  float *data = static_cast<float *>(nrrd->data);

  int dim_x = nrrd->axis[3].size;
  int dim_y = nrrd->axis[2].size;
  int dim_z = nrrd->axis[1].size;
  int dim_s = nrrd->axis[0].size;

  int step1 = dim_y * dim_z * dim_s;
  int step2 = dim_z * dim_s;

  // Iterate over all voxels and add noise.
  double mean;
  double std;
  double tmp;
  double voxel_sigma;
  for (int x = 0; x < dim_x; ++x) {
    for (int y = 0; y < dim_y; ++y) {
      for (int z = 0; z < dim_z; ++z) {
        // Calculate standard deviation at voxel;
        mean = 0.0;
        std = 0.0;

        for (int s = 0; s < dim_s; ++s) {
          mean += data[step1 * x + step2 * y + dim_s * z + s];
        }

        mean /= dim_s;
        //std::cout << mean << std::endl;

        for (int s = 0; s < dim_s; ++s) {
          tmp = data[step1 * x + step2 * y + dim_s * z + s] - mean;
          std += tmp * tmp;
        }

        std /= dim_s;
        std = sqrt(std);
        //std::cout << std << std::endl;

        // Caculate noise.
        voxel_sigma = std / sigma;
        for (int s = 0; s < dim_s; ++s) {
          double xx = voxel_sigma * RandomNormal();
          double yy = voxel_sigma * RandomNormal();
          //std::cout << "xx yy: " << xx / voxel_sigma << " " << yy / voxel_sigma << std::endl;

          tmp = data[step1 * x + step2 * y + dim_s * z + s];
          //std::cout << "val: " << tmp << std::endl;

          tmp += xx;
          tmp = sqrt(tmp * tmp + yy * yy);
          //std::cout << "new val: " << tmp << std::endl;

          // Make sure the signal value is not bigger than 1.
//          if (tmp > 1.0) {
//            tmp = 1.0;
//          }
          data[step1 * x + step2 * y + dim_s * z + s] = tmp;
        }
      }
    }
  }

  // Save the dwi data with addes noise.
  if (nrrdSave(output_file_name.c_str(), nrrd, NULL)) {
    err = biffGetDone(NRRD);
    std::cout << "Trouble writing " << output_file_name << ": " << err
              << std::endl;
    delete err;
    nrrdNuke(nrrd);
    return 1;
  }
  nrrdNuke(nrrd);

  return 0;
}

double RandomUniform() {
  return static_cast<double>(rand()) / RAND_MAX;
}

double RandomNormal() {
  double x1, x2, w;
          
  do {
    x1 = 2.0 * RandomUniform() - 1.0;
    x2 = 2.0 * RandomUniform() - 1.0;
    w = x1 * x1 + x2 * x2;
  } while (w >= 1.0);

  w = sqrt((-2.0 * log(w)) / w);
  return x1 * w;
}
