#include "tractography.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include "filter_model.h"
#include "signal_data.h"
#include "utilities.h"
#include "vtk_writer.h"


Tractography::Tractography(FilterModel *model, const std::string& output_file,
                           int num_tensors, bool is_full_model, double fa_min,
                           double ga_min, int seeds, double theta_max,
                           bool transform_position)
    : _ukf(NULL), _model(model), _output_file(output_file), _record_fa(false),
      _record_state(false), _record_cov(false), _num_tensors(num_tensors),
      _is_full_model(is_full_model), _fa_min(fa_min), _ga_min(ga_min),
      _seeds_per_voxel(seeds), _cos_theta_min(5.0), _cos_theta_max(theta_max),
      _p0(0.01), _sigma(1.0), _min_radius(0.87), _max_length(250),
      _full_brain(false), _full_brain_ga_min(0.18),
      _transform_position(transform_position) {
  if (_cos_theta_max != 0.0 && _cos_theta_max < _cos_theta_min) {
    std::cout << "Nonzero branching angle must be greater than 5 degrees."
              << std::endl;
    exit(1);
  }

  if (_num_tensors < 1 || _num_tensors > 3) {
    std::cout << "Only one, two or three tensors are supported." << std::endl;
    exit(1);
  }

  _cos_theta_max = cos(_cos_theta_max * M_PI / 180.0);
  _cos_theta_min = cos(_cos_theta_min * M_PI / 180.0);

  // Double check branching.
  _is_branching = _num_tensors > 1 && _cos_theta_max < 1.0;

  _dt = _num_tensors > 1 ? 0.2 : 0.3;
}

Tractography::~Tractography() {
  if (_signal_data) {
    delete _signal_data;
  }
  if (_ukf) {
    delete _ukf;
  }
}

bool Tractography::LoadFiles(const std::string& data_file,
                             const std::string& seed_file,
                             const std::string& mask_file) {
  _signal_data = new NrrdData(_sigma);

  if (seed_file.empty()) {
    _full_brain = true;
  }

  if (_signal_data->LoadData(data_file, seed_file, mask_file)) {
    std::cout << "SignalData could not be loaded" << std::endl;
    delete _signal_data;
    _signal_data = false;
    return true;
  }

  _model->set_signal_data(_signal_data);
  _model->set_signal_dim(_signal_data->GetSignalDimension() * 2);

  _ukf = new UnscentedKalmanFilter(_model);

  return false;
}

void Tractography::Init(std::vector<SeedPointInfo>& seed_infos) {
  assert(_signal_data);
  int signal_dim = _signal_data->GetSignalDimension();

  // Determinism.
  srand(0);

  // Create random offsets from the seed voxel.
  std::vector<vec_t> rand_dirs;
  for (int i = 0; i < _seeds_per_voxel; ++i) {
    vec_t dir = make_vec(static_cast<double>((rand() % 10001) - 5000),
                         static_cast<double>((rand() % 10001) - 5000),
                         static_cast<double>((rand() % 10001) - 5000));
    // HACK: If we want the first seed offset to be the same as in the Python
    // module we can overwrite the first random direction with the same random
    // values that are used in the Python module.
    //dir._[0] = 1.76405235;
    //dir._[1] = 0.40015721;
    //dir._[2] = 0.97873798;

    dir /= norm(dir);
    dir *= 0.5;

    rand_dirs.push_back(dir);
  }

  std::vector<vec_t> seeds;
  assert(_labels.size() > 0);
  if (!_full_brain) {
    _signal_data->GetSeeds(_labels, seeds);
  } else {
    // Iterate through all brain voxels and take those as seeds voxels.
    const vec_t dim = _signal_data->dim();
    for (int x = 0; x < dim._[0]; ++x) {
      for (int y = 0; y < dim._[1]; ++y) {
        for (int z = 0; z < dim._[2]; ++z) {
          vec_t pos = make_vec(x, y, z);
          if (_signal_data->Interp3ScalarMask(pos) > 0.1) {
            seeds.push_back(pos);
          }
        }
      }
    }
  }

  assert(seeds.size() > 0);

  // Calculate all starting points.
  std::vector<vec_t> starting_points;
  std::vector<std::vector<double> > signal_values;
  std::vector<double> signal;
  signal.resize(signal_dim * 2);

  std::vector<vec_t>::const_iterator cit;
  std::vector<vec_t>::iterator jt;
  int num_less_than_zero = 0;
  int num_invalid = 0;
  int num_ga_too_low = 0;
  for (cit = seeds.begin(); cit != seeds.end(); ++cit) {
    for (jt = rand_dirs.begin(); jt != rand_dirs.end(); ++jt) {
      vec_t point = *cit + *jt;
      _signal_data->Interp3Signal(point, signal);

      // Filter out all starting points that have negative signal values (due to
      // noise) or that otherwise have invalid signal values.
      bool keep = true;

      // We only scan half of the signal values since the second half is simply
      // a copy of the first half.
      for (int k = 0; k < signal_dim; ++k) {
        if (signal[k] < 0) {
          keep = false;
          ++num_less_than_zero;
          break;
        }
        if (isnan(signal[k]) || isinf(signal[k])) {
          keep = false;
          ++num_invalid;
          break;
        }

        // If we do full brain tractography we only want seed voxels where the
        // GA is bigger than 0.18.
        gmm::dense_matrix<double> signal_tmp(signal_dim * 2, 1);
        gmm::copy(signal, gmm::mat_col(signal_tmp, 0));
        if (_full_brain && s2ga(signal_tmp) < _full_brain_ga_min) {
          keep = false;
          ++num_ga_too_low;
          break;
        }
      }

      // If all the criteria is met we keep that point and the signal data.
      if (keep) {
        signal_values.push_back(signal);
        starting_points.push_back(point);
      }
    }
  }

  std::vector<std::vector<double> > starting_params(starting_points.size());
  UnpackTensor(_signal_data->GetAvgBValue(), _signal_data->gradients(),
               signal_values, starting_params);

  // If we work with the simple model we have to change the second and third
  // eigenvalues: l2 = l3 = (l2 + l3) / 2.
  if (!_is_full_model) {
    for (size_t i = 0; i < starting_params.size(); ++i) {
      starting_params[i][7] = starting_params[i][8] =
          (starting_params[i][7] + starting_params[i][8]) / 2.0;
    }
  }

  // Pack information for each seed point.
  int fa_too_low = 0;
  for (size_t i = 0; i < starting_points.size(); ++i) {
    const std::vector<double>& param = starting_params[i];
    assert(param.size() == 9);
    
    // Filter out seeds who's FA is too low.
    double fa = l2fa(param[6], param[7], param[8]);
    if (fa <= _fa_min) {
      ++fa_too_low;
      continue;
    }

    // Create seed info for both directions;
    SeedPointInfo info;
    SeedPointInfo info_inv;

    info.point = starting_points[i];
    info.start_dir = make_vec(param[0], param[1], param[2]);
    info.fa = fa;
    info_inv.point = starting_points[i];
    info_inv.start_dir = make_vec(-param[0], -param[1], -param[2]);
    info_inv.fa = fa;

    // Set the initial state.
    if (_is_full_model) {
      info.state.resize(6);
      info.state[0] = param[3]; // Theta
      info.state[1] = param[4]; // Phi
      info.state[2] = param[5]; // Psi
      info.state[5] = param[8]; // l3
      info_inv.state.resize(6);
      info_inv.state[0] = param[3]; // Theta
      info_inv.state[1] = param[4]; // Phi

      // Switch psi angle.
      // Careful here since M_PI is not standard c++.
      info_inv.state[2] = (param[5] < 0.0 ? param[5] + M_PI: param[5] - M_PI);
      info_inv.state[5] = param[8]; // l3
    } else {
      // Starting direction.
      info.state.resize(5);
      info.state[0] = info.start_dir._[0];
      info.state[1] = info.start_dir._[1];
      info.state[2] = info.start_dir._[2];
      info_inv.state.resize(5);
      info_inv.state[0] = info_inv.start_dir._[0];
      info_inv.state[1] = info_inv.start_dir._[1];
      info_inv.state[2] = info_inv.start_dir._[2];
    }
    info.state[3] = param[6]; // l1
    info.state[4] = param[7]; // l2
    info_inv.state[3] = param[6]; // l1
    info_inv.state[4] = param[7]; // l2

    // Duplicate/tripple states if we have several tensors.
    if (_num_tensors > 1) {
      size_t size = info.state.size();
      for (size_t j = 0 ; j < size; ++j) {
        info.state.push_back(info.state[j]);
        info_inv.state.push_back(info.state[j]);
      }
      if (_num_tensors > 2) {
        for (size_t j = 0 ; j < size; ++j) {
          info.state.push_back(info.state[j]);
          info_inv.state.push_back(info.state[j]);
        }
      }
    }

    // Create original covariance matrix;
    // If we have 2 or 3 tensors the state is duplicated or trippled.
    int state_dim = _is_full_model ? 6 : 5;
    if (_num_tensors == 2) {
      state_dim *= 2;
    } else if (_num_tensors == 3) {
      state_dim *= 3;
    }
    gmm::resize(info.covariance, state_dim, state_dim);
    gmm::resize(info_inv.covariance, state_dim, state_dim);
    for (int i = 0; i < state_dim; ++i) {
      info.covariance(i, i) = _p0;
      info_inv.covariance(i, i) = _p0;
    }

    seed_infos.push_back(info);
    seed_infos.push_back(info_inv);
  }
}

void Tractography::Run() {
  assert(_signal_data);

  // Initialize and prepare seeds.
  std::vector<SeedPointInfo> seed_infos;
  std::vector<SeedPointInfo> second_seed_infos;
  Init(seed_infos);

  clock_t start, finish;

  // Follow all the seed points.
  //PrintSeedInfo(seed_infos);
  start = clock();
  for (size_t i = 0; i < seed_infos.size(); ++i) {
    // Progress output.
    std::cout << "[" << std::setw(3) << (i + 1) * 100 / seed_infos.size()
              << "%]p (" << std::setw(7) << i + 1 << " - " << std::setw(7)
              << seed_infos.size() << ")" << std::endl;

    _fibers.push_back(Fiber());
    if (_num_tensors == 3) {
      Follow3T(seed_infos[i], _fibers[_fibers.size() - 1], _is_branching,
               second_seed_infos);
    } else if (_num_tensors == 2) {
      Follow2T(seed_infos[i], _fibers[_fibers.size() - 1], _is_branching,
               second_seed_infos);
    } else {
      Follow1T(seed_infos[i], _fibers[_fibers.size() - 1]);
    }
  }

  finish = clock();
  std::cout << "Time: " << static_cast<double>(finish - start) / CLOCKS_PER_SEC
            << "sec" << std::endl;

  // Seed from all the branches found.
  if (_is_branching) {
    start = clock();

    //PrintSeedInfo(second_seed_infos);
    for (size_t i = 0; i < second_seed_infos.size(); ++i) {

      // Progress output.
      std::cout << "[" << std::setw(3)
                << (i + 1) * 100 / second_seed_infos.size() << "%]s ("
                << std::setw(7) << i + 1 << " - " << std::setw(7)
                << second_seed_infos.size() << ")" << std::endl;

      assert(_num_tensors == 2 || _num_tensors == 3);
      _fibers.push_back(Fiber());
      if (_num_tensors == 3) {
          Follow3T(second_seed_infos[i], _fibers[_fibers.size() - 1], false,
                   second_seed_infos);
      } else if (_num_tensors == 2) {
          Follow2T(second_seed_infos[i], _fibers[_fibers.size() - 1], false,
                   second_seed_infos);
      }
    }

    finish = clock();
    std::cout << "Time: "
              << static_cast<double>(finish - start) / CLOCKS_PER_SEC << "sec"
              << std::endl;
  }

  // Write the fiber data to the output vtk file.
  VtkWriter writer(_signal_data);
  writer.set_transform_position(_transform_position);
  writer.Write(_output_file, _fibers);
}

void Tractography::UnpackTensor(double b, const std::vector<vec_t>& u,
                                std::vector<std::vector<double> >& s,
                                std::vector<std::vector<double> >& ret) {
  int signal_dim = _signal_data->GetSignalDimension();
  assert(ret.size() == s.size());

  // Build B matrix.
  gmm::dense_matrix<double> B(signal_dim * 2, 6);
  for (int i = 0; i < signal_dim * 2; ++i) {
    const vec_t& g = u[i];
    B(i, 0) = g._[0] * g._[0];
    B(i, 1) = 2.0 * g._[0] * g._[1];
    B(i, 2) = 2.0 * g._[0] * g._[2];
    B(i, 3) = g._[1] * g._[1];
    B(i, 4) = 2.0 * g._[1] * g._[2];
    B(i, 5) = g._[2] * g._[2];
  }
  gmm::scale(B, -b);

  // The six tensor components.
  std::vector<double> d(6);

  // Temporary variables.
  gmm::dense_matrix<double> D(3, 3);
  gmm::dense_matrix<double> Q(3, 3);
  gmm::dense_matrix<double> QT(3, 3);
  std::vector<double> sigma(3);
  double theta, phi, psi;

  // Unpack data.
  for (size_t i = 0; i < s.size(); ++i) {
    gmm::iteration iter(2.0e-10);
    for (size_t j = 0; j < s[i].size(); ++j) {
      s[i][j] = log(s[i][j]);
    }

    // Use least squares to find the matrix representation of the tensor at the
    // seed point of the fiber.
    gmm::least_squares_cg(B, d, s[i], iter);

    D(0, 0) = d[0]; D(0, 1) = d[1]; D(0, 2) = d[2];
    D(1, 0) = d[1]; D(1, 1) = d[3]; D(1, 2) = d[4];
    D(2, 0) = d[2]; D(2, 1) = d[4]; D(2, 2) = d[5];

    // Use singular value decomposition to extract the eigenvalues and the
    // rotation matrix (which gives us main direction of the tensor).
    gmm::svd(D, Q, QT, sigma);
    assert(sigma[0] >= sigma[1] && sigma[1] >= sigma[2]);
    if (gmm::lu_det(Q) < 0) {
      gmm::scale(Q, -1.0);
    }
    assert(gmm::lu_det(Q) > 0);

    // Extract the three Euler Angles from the rotation matrix.
    theta = acos(Q(2, 2));
    double epsilon = 1.0e-10;
    if (fabs(theta) > epsilon) {
      phi = atan2(Q(1, 2), Q(0, 2));
      psi = atan2(Q(2, 1), -Q(2, 0)); 
    } else {
      phi = atan2(-Q(0, 1), Q(1, 1));
      psi = 0.0;
    }

    ret[i].resize(9);
    ret[i][0] = Q(0, 0);
    ret[i][1] = Q(1, 0);
    ret[i][2] = Q(2, 0);
    ret[i][3] = theta;
    ret[i][4] = phi;
    ret[i][5] = psi;
    gmm::scale(sigma, 1.0e6);
    ret[i][6] = sigma[0];
    ret[i][7] = sigma[1];
    ret[i][8] = sigma[2];
  }
}

void Tractography::Follow3T(const SeedPointInfo& seed, Fiber& fiber, bool is_branching,
                            std::vector<SeedPointInfo>& branching_seeds) {
  assert(_model->signal_dim() == _signal_data->GetSignalDimension() * 2);

  // Unpack the seed information.
  vec_t x = seed.point;
  State state = seed.state;
  gmm::dense_matrix<double> p(seed.covariance);
  double fa = seed.fa;

  // Record start point.
  Record(x, fa, state, p, fiber);

  vec_t m1, l1, m2, l2, m3, l3;
  m1 = seed.start_dir;

  // Tract the fiber.
  gmm::dense_matrix<double> signal_tmp(_model->signal_dim(), 1);
  gmm::dense_matrix<double> state_tmp(_model->state_dim(), 1);
  int count = 0;
  int stepnr = 0;
  while (true) {
    ++stepnr;

    Step3T(x, m1, l1, m2, l2, m3, l3, fa, state, p);

    // Check if we should abort following this fiber. We abort if we reach the
    // CSF, if FA or GA get too small, if the curvature get's too high or if
    // the fiber gets too long.
    bool is_brain = _signal_data->Interp3ScalarMask(x) > 0.1;
    gmm::copy(state, gmm::mat_col(state_tmp, 0));
    _model->H(state_tmp, signal_tmp);
    double ga = s2ga(signal_tmp);
    bool in_csf = ga < _ga_min || fa < _fa_min;
    bool is_curving = curve_radius(fiber.position) < _min_radius;

    if (!is_brain || in_csf ||
        static_cast<int>(fiber.position.size()) > _max_length || is_curving) {
      if (static_cast<int>(fiber.position.size()) > _max_length) {
        std::cout << "Warning: wild fiber." << std::endl;
      }
      break;
    }

    // Record roughly once per voxel.
    if (count == static_cast<int>(round(1.0 / _dt))) {
      Record(x, fa, state, p, fiber);

      // Record branch if necessary.
      if (is_branching) {
        bool is_one = l1._[0] > l1._[1] && l1._[0] > l1._[2];
        is_one = is_one && l2fa(l1._[0], l1._[1], l1._[2]) > _fa_min;
        if (is_one) {
          bool add_m2 = false;
          bool add_m3 = false;

          bool is_two = l2._[0] > l2._[1] && l2._[0] > l2._[2];
          bool is_three = l3._[0] > l3._[1] && l3._[0] > l3._[2];
          is_two = is_two && l2fa(l2._[0], l2._[1], l2._[2]) > _fa_min;
          is_three = is_three && l2fa(l3._[0], l3._[1], l3._[2]) > _fa_min;

          bool is_branch1 =
              dot(m1, m2) < _cos_theta_min && dot(m1, m2) > _cos_theta_max;
          bool is_branch2 =
              dot(m1, m3) < _cos_theta_min && dot(m1, m3) > _cos_theta_max;
          bool is_branch3 =
              dot(m2, m3) < _cos_theta_min;

          int state_dim = _model->state_dim();
          // If there is a branch between m1 and m2.
          if (is_two && is_branch1) {
            // If there is a branch between m1 and m3 we have to check if we
            // branch twice or only once.
            if (is_three && is_branch2) {
              // If angle between m2 and m3 is big enough we have 2 branches.
              if (is_branch3) {
                add_m2 = true;
                add_m3 = true;

              } else {
                // Otherwise we only follow m2 or m3, and we follow the one
                // tensor where the FA is bigger.
                if (l2fa(l2._[0], l2._[1], l2._[2]) >
                    l2fa(l3._[0], l3._[1], l3._[2])) {
                  add_m2 = true;
                } else {
                  add_m3 = true;
                }
              }
            } else {
              // If it's not possible for m3 to branch we are sure that m2
              // branches.
              add_m2 = true;
            }
          } else if (is_three && is_branch2) {
            // If m2 is not branching we only check if m3 can branch.
            add_m3 = true;
          }

          // If we have to tensors and the angle between them is large enough we
          // create a new seed for the branch. Since this branch is following the
          // second tensor we swap the state and covariance.
          if (add_m2) {
            branching_seeds.push_back(SeedPointInfo());
            SeedPointInfo& seed = branching_seeds[branching_seeds.size() - 1];

            gmm::resize(seed.state, state_dim);
            gmm::copy(state, seed.state);
            gmm::resize(seed.covariance, state_dim, state_dim);
            gmm::copy(p, seed.covariance);
            SwapState3T(seed.state, seed.covariance, 2);
            seed.point = x;
            seed.start_dir = m2;
            seed.fa = l2fa(l2._[0], l2._[1], l2._[2]);
          }
          // Same for the third tensor.
          if (add_m3) {
            branching_seeds.push_back(SeedPointInfo());
            SeedPointInfo& seed = branching_seeds[branching_seeds.size() - 1];

            gmm::resize(seed.state, state_dim);
            gmm::copy(state, seed.state);
            gmm::resize(seed.covariance, state_dim, state_dim);
            gmm::copy(p, seed.covariance);
            SwapState3T(seed.state, seed.covariance, 3);
            seed.point = x;
            seed.start_dir = m3;
            seed.fa = l2fa(l3._[0], l3._[1], l3._[2]);
          }
        }
      }
      count = 0;
    } else {
      ++count;
    }
  }
}

void Tractography::Follow2T(const SeedPointInfo& seed, Fiber& fiber,
                            bool is_branching,
                            std::vector<SeedPointInfo>& branching_seeds) {
  // Unpack the seed information.
  vec_t x = seed.point;
  State state = seed.state;
  gmm::dense_matrix<double> p(seed.covariance);
  double fa = seed.fa;

  // Record start point.
  Record(x, fa, state, p, fiber);

  vec_t m1, l1, m2, l2;
  m1 = seed.start_dir;

  // Tract the fiber.
  gmm::dense_matrix<double> signal_tmp(_model->signal_dim(), 1);
  gmm::dense_matrix<double> state_tmp(_model->state_dim(), 1);
  int count = 0;
  int stepnr = 0;
  while (true) {
    ++stepnr;

    Step2T(x, m1, l1, m2, l2, fa, state, p);

    // Check if we should abort following this fiber. We abort if we reach the
    // CSF, if FA or GA get too small, if the curvature get's too high or if
    // the fiber gets too long.
    bool is_brain = _signal_data->Interp3ScalarMask(x) > 0.1;
    gmm::copy(state, gmm::mat_col(state_tmp, 0));
    _model->H(state_tmp, signal_tmp);
    double ga = s2ga(signal_tmp);
    bool in_csf = ga < _ga_min || fa < _fa_min;
    bool is_curving = curve_radius(fiber.position) < _min_radius;

    if (!is_brain || in_csf ||
        static_cast<int>(fiber.position.size()) > _max_length || is_curving) {
      if (static_cast<int>(fiber.position.size()) > _max_length) {
        std::cout << "Warning: wild fiber." << std::endl;
      }
      break;
    }

    // Record roughly once per voxel.
    if (count == static_cast<int>(round(1.0 / _dt))) {
      Record(x, fa, state, p, fiber);

      // Record branch if necessary.
      if (is_branching) {
        bool is_two = l1._[0] > l1._[1] && l1._[0] > l1._[2] &&
                      l2._[0] > l2._[1] && l2._[0] > l2._[2];
        is_two = is_two && l2fa(l1._[0], l1._[1], l1._[2]) > _fa_min &&
                           l2fa(l2._[0], l2._[1], l2._[2]) > _fa_min;
        double theta = dot(m1, m2);
        bool is_branch = theta < _cos_theta_min && theta > _cos_theta_max;

        // If we have to tensors and the angle between them is large enough we
        // create a new seed for the branch. Since this branch is following the
        // second tensor we swap the state and covariance.
        if (is_two && is_branch) {
          branching_seeds.push_back(SeedPointInfo());
          SeedPointInfo& seed = branching_seeds[branching_seeds.size() - 1];

          int state_dim = _model->state_dim();
          gmm::resize(seed.state, state_dim);
          gmm::copy(state, seed.state);
          gmm::resize(seed.covariance, state_dim, state_dim);
          gmm::copy(p, seed.covariance);
          SwapState2T(seed.state, seed.covariance);
          seed.point = x;
          seed.start_dir = m2;
          seed.fa = l2fa(l2._[0], l2._[1], l2._[2]);
        }
      }
      count = 0;
    } else {
      ++count;
    }
  }
}

// Also read the comments to Follow2T above, it's documented better than this
// function here.
void Tractography::Follow1T(const SeedPointInfo& seed, Fiber& fiber) {
  assert(_model->signal_dim() == _signal_data->GetSignalDimension() * 2);

  vec_t x = seed.point;
  State state = seed.state;

  gmm::dense_matrix<double> p(seed.covariance);

  double fa = seed.fa;

  // Record start point.
  Record(x, fa, state, p, fiber);

  // Tract the fiber.
  gmm::dense_matrix<double> signal_tmp(_model->signal_dim(), 1);
  gmm::dense_matrix<double> state_tmp(_model->state_dim(), 1);
  int count = 0;
  int stepnr = 0;
  while (true) {
    ++stepnr;
    Step1T(x, fa, state, p);

    // Terminate if off brain or in CSF.
    bool is_brain = _signal_data->Interp3ScalarMask(x) > 0.1;

    // This copy here might be unefficient :-(.
    gmm::copy(state, gmm::mat_col(state_tmp, 0));
    _model->H(state_tmp, signal_tmp);
    double ga = s2ga(signal_tmp);
    bool in_csf = ga < _ga_min || fa < _fa_min;
    bool is_curving = curve_radius(fiber.position) < _min_radius;

    if (!is_brain || in_csf ||
        static_cast<int>(fiber.position.size()) > _max_length || is_curving) {
      if (static_cast<int>(fiber.position.size()) > _max_length) {
        std::cout << "Warning: wild fiber." << std::endl;
      }
      break;
    }

    // Record roughly once per voxel.
    if (count == static_cast<int>(round(1.0 / _dt))) {
      Record(x, fa, state, p, fiber);
      count = 0;
    } else {
      ++count;
    }
  }
}

void Tractography::Step3T(vec_t& x, vec_t& m1, vec_t& l1, vec_t& m2, vec_t& l2,
                          vec_t& m3, vec_t& l3, double& fa, State& state,
                          gmm::dense_matrix<double>& covariance) {
  assert(static_cast<int>(gmm::mat_ncols(covariance)) == _model->state_dim() &&
         static_cast<int>(gmm::mat_ncols(covariance)) == _model->state_dim());
  assert(static_cast<int>(state.size()) == _model->state_dim());
  State state_new(_model->state_dim());
  gmm::dense_matrix<double> covariance_new(_model->state_dim(),
                                           _model->state_dim());

  // Use the Unscented Kalman Filter to get the next estimate.
  std::vector<double> signal(_signal_data->GetSignalDimension() * 2);
  _signal_data->Interp3Signal(x, signal);
  _ukf->Filter(state, covariance, signal, state_new, covariance_new);

  gmm::copy(state_new, state);
  gmm::copy(covariance_new, covariance);

  vec_t old_dir = m1;

  _model->State2Tensor(state, old_dir, m1, l1, m2, l2, m3, l3);
  double dot1 = dot(m1, old_dir);
  double dot2 = dot(m2, old_dir);
  double dot3 = dot(m3, old_dir);
  if (dot1 < dot2 && dot3 < dot2) {
    // Switch dirs and lambdas.
    vec_t tmp = m1;
    m1 = m2;
    m2 = tmp;
    tmp = l1;
    l1 = l2;
    l2 = tmp;

    // Swap state.
    SwapState3T(state, covariance, 2);
  } else if (dot1 < dot3) {
    // Switch dirs and lambdas.
    vec_t tmp = m1;
    m1 = m3;
    m3 = tmp;
    tmp = l1;
    l1 = l3;
    l3 = tmp;

    // Swap state.
    SwapState3T(state, covariance, 3);
  }

  // Update FA. If the first lamba is not the largest anymore the FA is set to
  // 0 what will lead to abortion in the tractography loop.
  if (l1._[0] < l1._[1] || l1._[0] < l1._[2]) {
    fa = 0.0;
  } else {
    fa = l2fa(l1._[0], l1._[1], l1._[2]);
  }

  vec_t voxel = _signal_data->voxel();
  vec_t dx = make_vec(m1._[2] / voxel._[2],
                      m1._[1] / voxel._[1],
                      m1._[0] / voxel._[0]);
  x = x + dx * _dt;
}

void Tractography::Step2T(vec_t& x, vec_t& m1, vec_t& l1, vec_t& m2, vec_t& l2,
                          double& fa, State& state,
                          gmm::dense_matrix<double>& covariance) {
  assert(static_cast<int>(gmm::mat_ncols(covariance)) == _model->state_dim() &&
         static_cast<int>(gmm::mat_ncols(covariance)) == _model->state_dim());
  assert(static_cast<int>(state.size()) == _model->state_dim());
  State state_new(_model->state_dim());
  gmm::dense_matrix<double> covariance_new(_model->state_dim(),
                                           _model->state_dim());

  // Use the Unscented Kalman Filter to get the next estimate.
  std::vector<double> signal(_signal_data->GetSignalDimension() * 2);
  _signal_data->Interp3Signal(x, signal);
  _ukf->Filter(state, covariance, signal, state_new, covariance_new);

  gmm::copy(state_new, state);
  gmm::copy(covariance_new, covariance);

  vec_t old_dir = m1;

  _model->State2Tensor(state, old_dir, m1, l1, m2, l2);
  if (dot(m1, old_dir) < dot(m2, old_dir)) {
    // Switch dirs and lambdas.
    vec_t tmp = m1;
    m1 = m2;
    m2 = tmp;
    tmp = l1;
    l1 = l2;
    l2 = tmp;

    // Swap state.
    SwapState2T(state, covariance);
  }

  // Update FA. If the first lamba is not the largest anymore the FA is set to
  // 0 what will lead to abortion in the tractography loop.
  if (l1._[0] < l1._[1] || l1._[0] < l1._[2]) {
    fa = 0.0;
  } else {
    fa = l2fa(l1._[0], l1._[1], l1._[2]);
  }

  vec_t voxel = _signal_data->voxel();
  vec_t dx = make_vec(m1._[2] / voxel._[2],
                      m1._[1] / voxel._[1],
                      m1._[0] / voxel._[0]);
  x = x + dx * _dt;
}

void Tractography::Step1T(vec_t& x, double& fa, State& state,
                          gmm::dense_matrix<double>& covariance) {
  assert(static_cast<int>(gmm::mat_ncols(covariance)) == _model->state_dim() &&
         static_cast<int>(gmm::mat_ncols(covariance)) == _model->state_dim());
  assert(static_cast<int>(state.size()) == _model->state_dim());
  State state_new(_model->state_dim());
  gmm::dense_matrix<double> covariance_new(_model->state_dim(),
                                           _model->state_dim());

  std::vector<double> signal(_signal_data->GetSignalDimension() * 2);
  _signal_data->Interp3Signal(x, signal);

  _ukf->Filter(state, covariance, signal, state_new, covariance_new);

  gmm::copy(state_new, state);
  gmm::copy(covariance_new, covariance);

  vec_t dir, l;
  _model->State2Tensor(state, dir, l);

  // Update FA. If the first lamba is not the largest anymore the FA is set to
  // 0 what will lead to abortion in the tractography loop.
  if (l._[0] < l._[1] || l._[0] < l._[2]) {
    fa = 0.0;
  } else {
    fa = l2fa(l._[0], l._[1], l._[2]);
  }

  vec_t voxel = _signal_data->voxel();
  vec_t dx = make_vec(dir._[2] / voxel._[2],
                      dir._[1] / voxel._[1],
                      dir._[0] / voxel._[0]);
  x = x + dx * _dt;
}

void Tractography::SwapState3T(State& state,
                               gmm::dense_matrix<double>& covariance, int i) {
  // This function is only for 3T.
  assert(i == 2 || i == 3);

  int state_dim = _model->state_dim();
  gmm::dense_matrix<double> tmp(state_dim, state_dim);
  state_dim /= 3; 
  assert(state_dim == 5 || state_dim == 6);
  --i;
  int j = i == 1 ? 2 : 1;
  i *= state_dim;
  j *= state_dim;

  gmm::copy(covariance, tmp);
  gmm::copy(gmm::sub_matrix(tmp, gmm::sub_interval(0, state_dim),
                            gmm::sub_interval(0, state_dim)),
            gmm::sub_matrix(covariance, gmm::sub_interval(i, state_dim),
                            gmm::sub_interval(i, state_dim)));
  gmm::copy(gmm::sub_matrix(tmp, gmm::sub_interval(i, state_dim),
                            gmm::sub_interval(i, state_dim)),
            gmm::sub_matrix(covariance, gmm::sub_interval(0, state_dim),
                            gmm::sub_interval(0, state_dim)));
  gmm::copy(gmm::sub_matrix(tmp, gmm::sub_interval(i, state_dim),
                            gmm::sub_interval(0, state_dim)),
            gmm::sub_matrix(covariance, gmm::sub_interval(0, state_dim),
                            gmm::sub_interval(i, state_dim)));
  gmm::copy(gmm::sub_matrix(tmp, gmm::sub_interval(0, state_dim),
                            gmm::sub_interval(i, state_dim)),
            gmm::sub_matrix(covariance, gmm::sub_interval(i, state_dim),
                            gmm::sub_interval(0, state_dim)));

  gmm::copy(gmm::sub_matrix(tmp, gmm::sub_interval(j, state_dim),
                            gmm::sub_interval(0, state_dim)),
            gmm::sub_matrix(covariance, gmm::sub_interval(j, state_dim),
                            gmm::sub_interval(i, state_dim)));
  gmm::copy(gmm::sub_matrix(tmp, gmm::sub_interval(j, state_dim),
                            gmm::sub_interval(i, state_dim)),
            gmm::sub_matrix(covariance, gmm::sub_interval(j, state_dim),
                            gmm::sub_interval(0, state_dim)));
  gmm::copy(gmm::sub_matrix(tmp, gmm::sub_interval(0, state_dim),
                            gmm::sub_interval(j, state_dim)),
            gmm::sub_matrix(covariance, gmm::sub_interval(i, state_dim),
                            gmm::sub_interval(j, state_dim)));
  gmm::copy(gmm::sub_matrix(tmp, gmm::sub_interval(i, state_dim),
                            gmm::sub_interval(j, state_dim)),
            gmm::sub_matrix(covariance, gmm::sub_interval(0, state_dim),
                            gmm::sub_interval(j, state_dim)));
            
  // Swap the state.
  std::vector<double> tmp_vec(state_dim * 3);
  gmm::copy(state, tmp_vec);
  gmm::copy(gmm::sub_vector(tmp_vec, gmm::sub_interval(0, state_dim)),
            gmm::sub_vector(state, gmm::sub_interval(i, state_dim)));
  gmm::copy(gmm::sub_vector(tmp_vec, gmm::sub_interval(i, state_dim)),
            gmm::sub_vector(state, gmm::sub_interval(0, state_dim)));
}

void Tractography::SwapState2T(State& state,
                             gmm::dense_matrix<double>& covariance) {
  // This function is only for 2T.

  int state_dim = _model->state_dim();
  gmm::dense_matrix<double> tmp(state_dim, state_dim);
  state_dim = state_dim >> 1; 

  gmm::copy(covariance, tmp);
  gmm::copy(gmm::sub_matrix(tmp, gmm::sub_interval(0, state_dim),
                            gmm::sub_interval(0, state_dim)),
            gmm::sub_matrix(covariance, gmm::sub_interval(state_dim, state_dim),
                            gmm::sub_interval(state_dim, state_dim)));
  gmm::copy(gmm::sub_matrix(tmp, gmm::sub_interval(state_dim, state_dim),
                            gmm::sub_interval(state_dim, state_dim)),
            gmm::sub_matrix(covariance, gmm::sub_interval(0, state_dim),
                            gmm::sub_interval(0, state_dim)));
  gmm::copy(gmm::sub_matrix(tmp, gmm::sub_interval(state_dim, state_dim),
                            gmm::sub_interval(0, state_dim)),
            gmm::sub_matrix(covariance, gmm::sub_interval(0, state_dim),
                            gmm::sub_interval(state_dim, state_dim)));
  gmm::copy(gmm::sub_matrix(tmp, gmm::sub_interval(0, state_dim),
                            gmm::sub_interval(state_dim, state_dim)),
            gmm::sub_matrix(covariance, gmm::sub_interval(state_dim, state_dim),
                            gmm::sub_interval(0, state_dim)));
            
  // Swap the state.
  std::vector<double> tmp_vec(state_dim * 2);
  gmm::copy(state, tmp_vec);
  gmm::copy(gmm::sub_vector(tmp_vec, gmm::sub_interval(0, state_dim)),
            gmm::sub_vector(state, gmm::sub_interval(state_dim, state_dim)));
  gmm::copy(gmm::sub_vector(tmp_vec, gmm::sub_interval(state_dim, state_dim)),
            gmm::sub_vector(state, gmm::sub_interval(0, state_dim)));
}

void Tractography::PrintSeedInfo(
    const std::vector<SeedPointInfo>& seed_infos) {
  for (size_t i = 0; i < seed_infos.size(); ++i) {
    const SeedPointInfo& inf = seed_infos[i];
    std::cout << "Seed point " << i << ":" << std::endl;
    std::cout << "  Pos: (" << inf.point._[0] << ", " << inf.point._[1] << ", "
              << inf.point._[2] << ")" << std::endl;
    std::cout << "  State:";
    for (size_t j = 0; j < inf.state.size(); ++j) {
      std::cout << " " << inf.state[j];
    }
    std::cout << std::endl;
    std::cout << "  Dir: (" << inf.start_dir._[0] << ", " << inf.start_dir._[1]
              << ", " << inf.start_dir._[2] << ")" << std::endl;
    std::cout << "  FA:" << inf.fa << std::endl;
  }
}

void Tractography::Record(const vec_t& x, double fa, const State& state,
                          const gmm::dense_matrix<double> p,
                          Fiber& fiber) {
  assert(_model->state_dim() == static_cast<int>(state.size()));
  assert(gmm::mat_nrows(p) == static_cast<unsigned int>(state.size()) &&
         gmm::mat_ncols(p) == static_cast<unsigned int>(state.size()));

  fiber.position.push_back(x);
  fiber.norm.push_back(gmm::mat_euclidean_norm(p));

  if (_record_fa) {
    fiber.fa.push_back(fa);
  }
  if (_record_state) {
    fiber.state.push_back(state);
  }
  if (_record_cov) {
    fiber.covariance.push_back(p);
  }
}
