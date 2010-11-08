#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <gmm/gmm.h>
#include "teem/nrrd.h"
#include "vtk_reader.h"
#include "../ukf/linalg.h"
#include "../ukf/signal_data.h"
#include "../ukf/utilities.h"


bool debug_output;
bool count_positives;
bool calc_fa_error;

const vec_t evs = make_vec(1700.0, 500.0, 300.0);

enum Label {
  NONE = 0,
  HORIZONTAL,
  VERTICAL,
  CROSSING
};

Label GetRegion(const Nrrd* label_nrrd, const vec_t& pos);

double AngularError(const vec_t& m1, const vec_t& m2, const vec_t& m1_correct,
                    const vec_t& m2_correct, double angle);
double AngularError(const vec_t& m1, const vec_t& m1_correct, double angle);

double CalculateNMSE(const std::vector<double>& signal_orig,
                     const std::vector<double>& signal_est);

void CalculateError(const NrrdData* signal, const Nrrd* label_nrrd,
                    const VtkReader *vtk, double angle);

void CalculateEstimatedSignal(const NrrdData *signal_data,
                              const std::vector<double>& state,
                              std::vector<double>& signal);

int main(int argc, char** argv) {
  std::string help_string =
      std::string("Usage:\n") + argv[0] + " --fiber_file FILE --dwi_file FILE"
      " --region_file FILE --angle INT [--debug (true|false)]"
      " [--count_positives (true|false)] [--calc_fa_error (true|false)]";

  if (argc != 9 && argc != 11 && argc != 13 && argc != 15) {
    std::cout << "Error!" << std::endl;
    std::cout << help_string << std::endl;
    return 1;
  }

  const char *fiber_file = NULL;
  const char *dwi_file = NULL;
  const char *region_file = NULL;
  int angle_int = -1;

  // Parse command line arguments.
  debug_output = false;
  bool error = false;
  count_positives = true;
  calc_fa_error = false;
  int i = 1;
  if (argc % 2) {
    while (i + 1 < argc) {
      if (!std::string(argv[i]).compare("--fiber_file")) {
        fiber_file = argv[++i];
      } else if (!std::string(argv[i]).compare("--dwi_file")) {
        dwi_file = argv[++i];
      } else if (!std::string(argv[i]).compare("--region_file")) {
        region_file = argv[++i];
      } else if (!std::string(argv[i]).compare("--angle")) {
        angle_int = atoi(argv[++i]);
      } else if (!std::string(argv[i]).compare("--debug")) {
        if (!std::string(argv[++i]).compare("true")) {
          debug_output = true;
        } else if (std::string(argv[i]).compare("false")) {
          error = true;
        }
      } else if (!std::string(argv[i]).compare("--count_positives")) {
        if (!std::string(argv[++i]).compare("false")) {
          count_positives = false;
        } else if (std::string(argv[i]).compare("true")) {
          error = true;
        }
      } else if (!std::string(argv[i]).compare("--calc_fa_error")) {
        if (!std::string(argv[++i]).compare("true")) {
          calc_fa_error = true;
        } else if (std::string(argv[i]).compare("false")) {
          error = true;
        }
      } else {
        error = true;
        break;
      }
      ++i;
    }
  }

  if (!fiber_file || !dwi_file || !region_file || angle_int < 0 ||
      angle_int > 90 || error) {
    std::cout << "Error!" << std::endl << std::endl;
    std::cout << help_string << std::endl;
    return 1;
  }

  double angle = static_cast<double>(angle_int);
  assert(angle >= 00.0);
  assert(angle <= 90.0);

  std::string input_vtk(fiber_file);
  std::string input_nrrd(dwi_file);
  std::string label_nrrd_file(region_file);

  Nrrd *label_nrrd = nrrdNew();
  char *err;
  if (nrrdLoad(label_nrrd, label_nrrd_file.c_str(), NULL)) {
    err = biffGetDone(NRRD);
    std::cout << "Trouble reading " << label_nrrd_file << ": " << err
              << std::endl;
    delete err;
    nrrdNuke(label_nrrd);
    return 1;
  }

  // Load the avarage b value and the gradients from the signal.
  const double sigma = 1.0;
  NrrdData signal(sigma);

  if (signal.LoadSignal(input_nrrd.c_str())) {
    std::cout << "Signal could not be loaded" << std::endl;
    nrrdNuke(label_nrrd);
    return 1;
  }

  VtkReader reader;
  reader.Read(input_vtk);

  CalculateError(&signal, label_nrrd, &reader, angle);

  nrrdNuke(label_nrrd);

  return 0;
}

// This does not interpolate.
Label GetRegion(const Nrrd* label_nrrd, const vec_t& pos) {
  // Go through the volume.
  int nx = label_nrrd->axis[2].size;
  int ny = label_nrrd->axis[1].size;
  int nz = label_nrrd->axis[0].size;

  int data_type = label_nrrd->type;

  int x = static_cast<int>(round(pos._[0]));
  int y = static_cast<int>(round(pos._[1]));
  int z = static_cast<int>(round(pos._[2]));
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


  if (debug_output) {
    std::cout << "pos: " << x << " " << y << " " << z << std::endl;
  }

  int index = ny * nz * x + nz * y + z;
  int value = 0;
  // TODO: Maybe add more data types or make this more generic.
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

  // Figure out if we're in the horizontal or vertical region.
  Label ret = static_cast<Label>(value);
  if (ret == 1) {
    if (y > 40 || y < 20) {
      ret = VERTICAL;
    }
  }

  assert(ret == 0 || ret == 1 || ret == 2 || ret == 3);

  return ret;
}

double AngularError(const vec_t& m1, const vec_t& m2, const vec_t& m1_correct,
                    const vec_t& m2_correct, double angle) {
  double e1_1 = acos(fabs(dot(m1, m1_correct)));
  double e1_2 = acos(fabs(dot(m2, m2_correct)));
  double e2_1 = acos(fabs(dot(m1, m2_correct)));
  double e2_2 = acos(fabs(dot(m2, m1_correct)));

  double error1 = e1_1 + e1_2;
  double error2 = e2_1 + e2_2;

  if (debug_output) {
    std::cout << "error 2t: " << error1 << " " << error2 << std::endl;
  }

  if (error1 < error2) {
    return error1 / 2.0;
  } else {
    return error2 / 2.0;
  }
}

double AngularError(const vec_t& m1, const vec_t& m1_correct, double angle) {
  return acos(fabs(dot(m1, m1_correct)));
}

double CalculateNMSE(const std::vector<double>& signal_orig,
                     const std::vector<double>& signal_est) {
  assert(signal_orig.size() == signal_est.size());
  std::vector<double> tmp(signal_orig.size());

  gmm::add(signal_est, gmm::scaled(signal_orig, -1), tmp);

  double n1 = gmm::vect_norm2(tmp);
  double n2 = gmm::vect_norm2(signal_orig);

  return n1 * n1 / (n2 * n2);
}

void CalculateError(const NrrdData* signal, const Nrrd* label_nrrd,
                    const VtkReader *vtk, double angle) {
  const std::vector<vec_t>& points = vtk->points();
  const std::vector<std::vector<unsigned int> >& lines = vtk->lines();
  const std::vector<std::vector<double> >& states = vtk->states();

  int signal_dim = 2 * signal->GetSignalDimension();
  std::vector<double> signal_orig(signal_dim);
  std::vector<double> signal_est(signal_dim);

  std::vector<double> angular_error;
  std::vector<double> angular_error_crossing;
  std::vector<double> nmse;
  std::vector<double> fa_error;

  const double fa_correct = l2fa(evs._[0], evs._[1], evs._[2]);

  int count = 0;
  int positives = 0; 

  for (size_t i = 0; i < lines.size(); ++i) {
    // Expected start direction. We use this to skip fibers who go into the
    // other direction.
    vec_t start_dir = make_vec(0.0, 0.0, -1.0);

    const vec_t& p0 = points[lines[i][0]];
    const vec_t& p1 = points[lines[i][1]];
    vec_t dir = p1 - p0;

    // Skip the fiber if it goes into the wrong direction.
    if (dot(start_dir, dir) < 0) {
      continue;
    }

    ++count;

    // Iterate through the fiber.
    for (size_t j = 0; j < lines[i].size(); ++j) {
      unsigned int index = lines[i][j];
      const vec_t& point = points[index];
      const std::vector<double>& state = states[index];

      // Find the region that we're in.
      Label region = GetRegion(label_nrrd, point);

      if (debug_output) {
        std::cout << "region: " << region << std::endl;
      }

      if (region != NONE) {
        // TODO: Only get error in a selected region.

        // Now get/calculate how the state/direction in this region is supposed
        // to be.
        vec_t m1_correct;
        vec_t m2_correct;
        if (region == HORIZONTAL) {
          m1_correct = make_vec(0.0, 0.0, -1.0);
          m2_correct = make_vec(0.0, 0.0, -1.0);
        } else if (region == CROSSING) {
          double y = sin(angle * M_PI / 180.0);
          double x = cos(angle * M_PI / 180.0);
          m1_correct = make_vec(0.0, 0.0, -1.0);
          m2_correct = make_vec(0.0, y, x);
        } else if (region == VERTICAL) {
          double y = sin(angle * M_PI / 180.0);
          double x = cos(angle * M_PI / 180.0);
          m1_correct = make_vec(0.0, y, x);
          m2_correct = make_vec(0.0, y, x);
        } else {
          std::cout << region << std::endl;
          assert(false);
        }

        if (debug_output) {
          std::cout << "m1 correct: " << m1_correct._[0] << " "
                    << m1_correct._[1] << " " << m1_correct._[2] << std::endl;
          if (state.size() > 6) {
            std::cout << "m2 correct: " << m2_correct._[0] << " "
                      << m2_correct._[1] << " " << m2_correct._[2] << std::endl;
          }
        }

        // Get direction as we measured it.
        vec_t m1;
        vec_t m2;
        assert(state.size() == 5 || state.size() == 6 || state.size() == 10 ||
               state.size() == 12/* || state.size() == 15 || state.size() == 18*/);
        // Currently we only do the error calculations for 2-tensor models.
        if (state.size() == 5) {
          m1 = make_vec(state[0], state[1], state[2]);
        } else if (state.size() == 6) {
          m1 = rotation_main_dir(state[0], state[1], state[2]);
        } else if (state.size() == 10) {
          m1 = make_vec(state[0], state[1], state[2]);
          m2 = make_vec(state[5], state[6], state[7]);
        } else if (state.size() == 12) {
          m1 = rotation_main_dir(state[0], state[1], state[2]);
          m2 = rotation_main_dir(state[6], state[7], state[8]);
        }
        double tmp = m1._[0];
        m1._[0] = m1._[2];
        m1._[2] = tmp;
        tmp = m2._[0];
        m2._[0] = m2._[2];
        m2._[2] = tmp;

        if (debug_output) {
          std::cout << "m1: " << m1._[0] << " " << m1._[1] << " " << m1._[2]
                    << std::endl;
          if (state.size() > 6) {
            std::cout << "m2: " << m2._[0] << " " << m2._[1] << " " << m2._[2]
                      << std::endl;
          }
          if (state.size() == 6) {
            std::cout << "angles (in radiants): " << state[0] << " " << state[1]
                      << " " << state[2] << std::endl;
            std::cout << "angles (in degree): " << state[0] * 180.0 / M_PI
                      << " " << state[1] * 180.0 / M_PI << " "
                      << state[2] * 180.0 / M_PI << std::endl;
          }
          if (state.size() == 12) {
            std::cout << "angles (in radiants): " << state[0] << " " << state[1]
                      << " " << state[2] << " " << state[6] << " " << state[7]
                      << " " << state[8] << std::endl;
            std::cout << "angles (in degree): " << state[0] * 180.0 / M_PI
                      << " " << state[1] * 180.0 / M_PI << " "
                      << state[2] * 180.0 / M_PI << " "
                      << state[6] * 180.0 / M_PI << " "
                      << state[7] * 180.0 / M_PI << " "
                      << state[8] * 180.0 / M_PI << std::endl;
          }
        }

        double error;
        if (state.size() == 5 || state.size() == 6) {
          error = AngularError(m1, m1_correct, angle);
        } else {
          error = AngularError(m1, m2, m1_correct, m2_correct, angle);
        }

        if (region == CROSSING) {
          angular_error_crossing.push_back(error);
        } else {
          angular_error.push_back(error);
        }

        if (debug_output) {
          std::cout << "angular error (in radiants): " << error << std::endl;
          std::cout << "angular error (in degree): " << error * 180.0 / M_PI
                    << std::endl;
        }

        signal->Interp3Signal(point, signal_orig);
        CalculateEstimatedSignal(signal, state, signal_est);

        if (debug_output) {
          std::cout << "nmse: " << CalculateNMSE(signal_orig, signal_est)
                    << std::endl << std::endl;
        }

        nmse.push_back(CalculateNMSE(signal_orig, signal_est));

        // Calculate FA error.
        if (calc_fa_error) {
          double fa = 0.0;
          if (state.size() == 5 || state.size() == 10) {
            fa = l2fa(state[3], state[4], state[4]);
          } else if (state.size() == 6 || state.size() == 12) {
            fa = l2fa(state[3], state[4], state[6]);
          } else {
            assert(true);
          }
          fa_error.push_back(fabs(fa_correct - fa));
          if (debug_output) {
            std::cout << "FA (FA, correct FA, error): " << fa << ", "
                      << fa_correct << ", " << fa_error[fa_error.size() - 1]
                      << std::endl;
          }
        }
      }

      // We reach the last point, now check if we landed in the correct region.
      if (j == lines[i].size() - 1) {
        // The fibers start with a z-value of ~60 and it should decrease to 0.
        // We assume that if it gets < 2 that we arrived in the desired area.
        if (region == HORIZONTAL && point._[2] < 2) {
          ++positives;
        }
        if (angle < 30 && region == CROSSING && point._[2] < 2) {
          ++positives;
        }
      }
    }
  }

  // Process the collected error data.
  double pos_percentage = static_cast<double>(positives) / count * 100.0;
  if (count_positives) {
    if (debug_output) {
      std::cout << "Positives: " << positives << " of " << count << " ("
                << pos_percentage << "%)" << std::endl;
    }
  }

  double ang_err_non_cross = 0.0;
  if (angular_error.size()) {
    for (size_t i = 0; i < angular_error.size(); ++i) {
      ang_err_non_cross += angular_error[i];
    }
    ang_err_non_cross /= angular_error.size();
  } else {
    ang_err_non_cross = -1.0;
  }
  
  if (debug_output) {
    std::cout << "Angular error in non-crossing region (in radiants): "
              << ang_err_non_cross << std::endl;
    std::cout << "Angular error in non-crossing region (in degree): "
              << ang_err_non_cross * 180.0 / M_PI << std::endl;
  }
  double ang_err_cross = 0.0;
  if (angular_error_crossing.size()) {
    for (size_t i = 0; i < angular_error_crossing.size(); ++i) {
      ang_err_cross += angular_error_crossing[i];
    }
    ang_err_cross /= angular_error_crossing.size();
  } else {
    ang_err_cross = -1.0 * M_PI / 180.0;
  }
  if (debug_output) {
    std::cout << "Angular error in crossing region (in radiants): "
              << ang_err_cross << std::endl;
    std::cout << "Angular error in crossing region (in degree): "
              << ang_err_cross * 180.0 / M_PI << std::endl;
  }

  double nmse_avg = 0.0;
  for (size_t i = 0; i < nmse.size(); ++i) {
    nmse_avg += nmse[i];
  }
  nmse_avg /= nmse.size();
  if (debug_output) {
    std::cout << "Normalized Mean Square Error (NMSE): " << nmse_avg
              << std::endl << std::endl;
  }

  double fa_error_avg = 0.0;
  if (calc_fa_error) {
    for (size_t i = 0; i < fa_error.size(); ++i) {
      fa_error_avg += fa_error[i];
    }
    fa_error_avg /= fa_error.size();
    if (debug_output) {
      std::cout << "FA error: " << fa_error_avg << std::endl << std::endl;
    }
  }

  std::cout << angle << "\t";
  if (count_positives) {
    std::cout << pos_percentage << "\t";
  }
  std::cout << ang_err_non_cross * 180.0 / M_PI << "\t"
            << ang_err_cross * 180.0 / M_PI << "\t" << nmse_avg;
  if (calc_fa_error) {
    std::cout << "\t" << fa_error_avg;
  }
  std::cout << std::endl;
}

void CalculateEstimatedSignal(const NrrdData *signal_data,
                              const std::vector<double>& state,
                              std::vector<double>& signal) {
  const double lambda_min = 100.0;
  double b = signal_data->GetAvgBValue();
  const std::vector<vec_t>& gradients = signal_data->gradients();

  if (state.size() == 12) {
    // Clamp lambdas.
    double l11 = std::max(state[3], lambda_min);
    double l12 = std::max(state[4], lambda_min);
    double l13 = std::max(state[5], lambda_min);
    double l21 = std::max(state[9], lambda_min);
    double l22 = std::max(state[10], lambda_min);
    double l23 = std::max(state[11], lambda_min);
    
    // Calculate diffusion matrix.
    mat_t D1 = diffusion_euler(state[0], state[1], state[2], l11, l12, l13);
    mat_t D2 = diffusion_euler(state[6], state[7], state[8], l21, l22, l23);
  
    // Reconstruct signal.
    for (int i = 0; i < signal_data->GetSignalDimension() * 2; ++i) {
      const vec_t& u = gradients[i];
      signal[i] = (exp(-b * dot(u, D1 * u)) + exp(-b * dot(u, D2 * u))) / 2.0;
    }
  } else if (state.size() == 10) {
    // Normalize directions.
    vec_t m1 = make_vec(state[0], state[1], state[2]);
    vec_t m2 = make_vec(state[5], state[6], state[7]);
    m1 /= norm(m1);
    m2 /= norm(m2);

    // Clamp lambdas.
    double l11 = std::max(state[3], lambda_min);
    double l12 = std::max(state[4], lambda_min);
    double l21 = std::max(state[8], lambda_min);
    double l22 = std::max(state[9], lambda_min);

    // Flip if necessary.
    if (m1._[0] < 0) {
      m1 = -m1;
    }
    if (m2._[0] < 0) {
      m2 = -m2;
    }

    // Calculate diffusion matrix.
    mat_t D1 = diffusion(m1, l11, l12);
    mat_t D2 = diffusion(m2, l21, l22);

    // Reconstruct signal.
    for (int i = 0; i < signal_data->GetSignalDimension() * 2; ++i) {
      const vec_t& u = gradients[i];
      signal[i] = (exp(-b * dot(u, D1 * u)) + exp(-b * dot(u, D2 * u))) / 2.0;
    }
  } else if (state.size() == 6) {
    // Clamp lambdas.
    double l1 = std::max(state[3], lambda_min);
    double l2 = std::max(state[4], lambda_min);
    double l3 = std::max(state[5], lambda_min);
    
    // Calculate diffusion matrix.
    mat_t D = diffusion_euler(state[0], state[1], state[2], l1, l2, l3);
  
    // Reconstruct signal.
    for (int i = 0; i < signal_data->GetSignalDimension() * 2; ++i) {
      const vec_t& u = gradients[i];
      signal[i] = exp(-b * dot(u, D * u));
    }
  } else if (state.size() == 5) {
    // Normalize directions.
    vec_t m = make_vec(state[0], state[1], state[2]);
    m /= norm(m);

    // Clamp lambdas.
    double l1 = std::max(state[3], lambda_min);
    double l2 = std::max(state[4], lambda_min);

    // Flip if necessary.
    if (m._[0] < 0) {
      m = -m;
    }

    // Calculate diffusion matrix.
    mat_t D = diffusion(m, l1, l2);

    // Reconstruct signal.
    for (int i = 0; i < signal_data->GetSignalDimension() * 2; ++i) {
      const vec_t& u = gradients[i];
      signal[i] = exp(-b * dot(u, D * u));
    }
  } else {
    assert(false);
  }
}
