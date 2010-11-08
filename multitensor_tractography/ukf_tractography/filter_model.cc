#include "filter_model.h"
#include <iostream>

// Functions for 1-tensor full model.
void Full1T::F(gmm::dense_matrix<double>& X) {
  assert(_signal_dim > 0);
  assert(gmm::mat_nrows(X) == static_cast<unsigned int>(_state_dim) &&
         gmm::mat_ncols(X) == static_cast<unsigned int>(2 * _state_dim + 1));

  // Clamp lambdas.
  for (size_t i = 0; i < gmm::mat_ncols(X); ++i) {
    X(3, i) = std::max(X(3, i), _lambda_min);
    X(4, i) = std::max(X(4, i), _lambda_min);
    X(5, i) = std::max(X(5, i), _lambda_min);
  }
}

void Full1T::H(const gmm::dense_matrix<double>& X,
               gmm::dense_matrix<double>& Y) {
  assert(_signal_dim > 0);
  assert(gmm::mat_nrows(X) == static_cast<unsigned int>(_state_dim) &&
         (gmm::mat_ncols(X) == static_cast<unsigned int>(2 * _state_dim + 1) ||
          gmm::mat_ncols(X) == 1));
  assert(gmm::mat_nrows(Y) == static_cast<unsigned int>(_signal_dim) &&
         (gmm::mat_ncols(Y) == static_cast<unsigned int>(2 * _state_dim + 1) ||
          gmm::mat_ncols(Y) == 1));
  assert(_signal_data);
  double b = _signal_data->GetAvgBValue();
  const std::vector<vec_t>& gradients = _signal_data->gradients();

  for (size_t i = 0; i < gmm::mat_ncols(X); ++i) {
    // Clamp lambdas.
    double l1 = std::max(X(3, i), _lambda_min);
    double l2 = std::max(X(4, i), _lambda_min);
    double l3 = std::max(X(5, i), _lambda_min);
    
    // Calculate diffusion matrix.
    mat_t D = diffusion_euler(X(0, i), X(1, i), X(2, i), l1, l2, l3);

    // Reconstruct signal.
    for (int j = 0; j < _signal_dim; ++j) {
      const vec_t& u = gradients[j];
      Y(j, i) = exp(-b * dot(u, D * u));
    }
  }
}

void Full1T::State2Tensor(const State& x, vec_t& m, vec_t& l) {
  // Orientation.
  m = rotation_main_dir(x[0], x[1], x[2]);

  // Clamp lambdas.
  l._[0] = std::max(x[3], _lambda_min);
  l._[1] = std::max(x[4], _lambda_min);
  l._[2] = std::max(x[5], _lambda_min);
}

// Functions for 2-tensor full model.
void Full2T::F(gmm::dense_matrix<double>& X) {
  assert(_signal_dim > 0);
  assert(gmm::mat_nrows(X) == static_cast<unsigned int>(_state_dim) &&
         gmm::mat_ncols(X) == static_cast<unsigned int>(2 * _state_dim + 1));

  // Clamp lambdas.
  for (size_t i = 0; i < gmm::mat_ncols(X); ++i) {
    X(3, i) = std::max(X(3, i), _lambda_min);
    X(4, i) = std::max(X(4, i), _lambda_min);
    X(5, i) = std::max(X(5, i), _lambda_min);
    X(9, i) = std::max(X(9, i), _lambda_min);
    X(10, i) = std::max(X(10, i), _lambda_min);
    X(11, i) = std::max(X(11, i), _lambda_min);
  }
}

void Full2T::H(const gmm::dense_matrix<double>& X,
               gmm::dense_matrix<double>& Y) {
  assert(_signal_dim > 0);
  assert(gmm::mat_nrows(X) == static_cast<unsigned int>(_state_dim) &&
         (gmm::mat_ncols(X) == static_cast<unsigned int>(2 * _state_dim + 1) ||
          gmm::mat_ncols(X) == 1));
  assert(gmm::mat_nrows(Y) == static_cast<unsigned int>(_signal_dim) &&
         (gmm::mat_ncols(Y) == static_cast<unsigned int>(2 * _state_dim + 1) ||
          gmm::mat_ncols(Y) == 1));
  assert(_signal_data);
  double b = _signal_data->GetAvgBValue();
  const std::vector<vec_t>& gradients = _signal_data->gradients();

  for (size_t i = 0; i < gmm::mat_ncols(X); ++i) {
    // Clamp lambdas.
    double l11 = std::max(X(3, i), _lambda_min);
    double l12 = std::max(X(4, i), _lambda_min);
    double l13 = std::max(X(5, i), _lambda_min);
    double l21 = std::max(X(9, i), _lambda_min);
    double l22 = std::max(X(10, i), _lambda_min);
    double l23 = std::max(X(11, i), _lambda_min);
    
    // Calculate diffusion matrix.
    mat_t D1 = diffusion_euler(X(0, i), X(1, i), X(2, i), l11, l12, l13);
    mat_t D2 = diffusion_euler(X(6, i), X(7, i), X(8, i), l21, l22, l23);

    // Reconstruct signal.
    for (int j = 0; j < _signal_dim; ++j) {
      const vec_t& u = gradients[j];
      Y(j, i) = (exp(-b * dot(u, D1 * u)) + exp(-b * dot(u, D2 * u))) / 2.0;
    }
  }
}

void Full2T::State2Tensor(const State& x, const vec_t& old_m, vec_t& m1,
                          vec_t& l1, vec_t& m2, vec_t& l2) {
  // First orientation.
  m1 = rotation_main_dir(x[0], x[1], x[2]);

  // Flip orientation if necessary. (For m1 it should not happen, maybe for
  // m2.)
  if (m1._[0] * old_m._[0] + m1._[1] * old_m._[1] + m1._[2] * old_m._[2] < 0) {
    m1 = -m1;
  }

  // Clamp lambdas.
  l1._[0] = std::max(x[3], _lambda_min);
  l1._[1] = std::max(x[4], _lambda_min);
  l1._[2] = std::max(x[5], _lambda_min);

  // Second orientation.
  m2 = rotation_main_dir(x[6], x[7], x[8]);

  // Flip orientation if necessary.
  if (m2._[0] * old_m._[0] + m2._[1] * old_m._[1] + m2._[2] * old_m._[2] < 0) {
    m2 = -m2;
  }

  // Clamp lambdas.
  l2._[0] = std::max(x[9], _lambda_min);
  l2._[1] = std::max(x[10], _lambda_min);
  l2._[2] = std::max(x[11], _lambda_min);
}

// Functions for 3-tensor full model.
void Full3T::F(gmm::dense_matrix<double>& X) {
  assert(_signal_dim > 0);
  assert(gmm::mat_nrows(X) == static_cast<unsigned int>(_state_dim) &&
         gmm::mat_ncols(X) == static_cast<unsigned int>(2 * _state_dim + 1));

  // Clamp lambdas.
  for (size_t i = 0; i < gmm::mat_ncols(X); ++i) {
    X(3, i) = std::max(X(3, i), _lambda_min);
    X(4, i) = std::max(X(4, i), _lambda_min);
    X(5, i) = std::max(X(5, i), _lambda_min);
    X(9, i) = std::max(X(9, i), _lambda_min);
    X(10, i) = std::max(X(10, i), _lambda_min);
    X(11, i) = std::max(X(11, i), _lambda_min);
    X(15, i) = std::max(X(15, i), _lambda_min);
    X(16, i) = std::max(X(16, i), _lambda_min);
    X(17, i) = std::max(X(17, i), _lambda_min);
  }
}

void Full3T::H(const gmm::dense_matrix<double>& X,
               gmm::dense_matrix<double>& Y) {
  assert(_signal_dim > 0);
  assert(gmm::mat_nrows(X) == static_cast<unsigned int>(_state_dim) &&
         (gmm::mat_ncols(X) == static_cast<unsigned int>(2 * _state_dim + 1) ||
          gmm::mat_ncols(X) == 1));
  assert(gmm::mat_nrows(Y) == static_cast<unsigned int>(_signal_dim) &&
         (gmm::mat_ncols(Y) == static_cast<unsigned int>(2 * _state_dim + 1) ||
          gmm::mat_ncols(Y) == 1));
  assert(_signal_data);
  double b = _signal_data->GetAvgBValue();
  const std::vector<vec_t>& gradients = _signal_data->gradients();

  for (size_t i = 0; i < gmm::mat_ncols(X); ++i) {
    // Clamp lambdas.
    double l11 = std::max(X(3, i), _lambda_min);
    double l12 = std::max(X(4, i), _lambda_min);
    double l13 = std::max(X(5, i), _lambda_min);
    double l21 = std::max(X(9, i), _lambda_min);
    double l22 = std::max(X(10, i), _lambda_min);
    double l23 = std::max(X(11, i), _lambda_min);
    double l31 = std::max(X(15, i), _lambda_min);
    double l32 = std::max(X(16, i), _lambda_min);
    double l33 = std::max(X(17, i), _lambda_min);
    
    // Calculate diffusion matrix.
    mat_t D1 = diffusion_euler(X(0, i), X(1, i), X(2, i), l11, l12, l13);
    mat_t D2 = diffusion_euler(X(6, i), X(7, i), X(8, i), l21, l22, l23);
    mat_t D3 = diffusion_euler(X(12, i), X(13, i), X(14, i), l31, l32, l33);

    // Reconstruct signal.
    for (int j = 0; j < _signal_dim; ++j) {
      const vec_t& u = gradients[j];
      Y(j, i) = (exp(-b * dot(u, D1 * u)) +
                 exp(-b * dot(u, D2 * u)) +
                 exp(-b * dot(u, D3 * u))) / 3.0;
    }
  }
}

void Full3T::State2Tensor(const State& x, const vec_t& old_m, vec_t& m1,
                          vec_t& l1, vec_t& m2, vec_t& l2, vec_t& m3,
                          vec_t& l3) {
  // First orientation.
  m1 = rotation_main_dir(x[0], x[1], x[2]);

  // Flip orientation if necessary. (For m1 it should not happen, maybe for
  // m2.)
  if (m1._[0] * old_m._[0] + m1._[1] * old_m._[1] + m1._[2] * old_m._[2] < 0) {
    m1 = -m1;
  }

  // Clamp lambdas.
  l1._[0] = std::max(x[3], _lambda_min);
  l1._[1] = std::max(x[4], _lambda_min);
  l1._[2] = std::max(x[5], _lambda_min);

  // Second orientation.
  m2 = rotation_main_dir(x[6], x[7], x[8]);

  // Flip orientation if necessary.
  if (m2._[0] * old_m._[0] + m2._[1] * old_m._[1] + m2._[2] * old_m._[2] < 0) {
    m2 = -m2;
  }

  // Clamp lambdas.
  l2._[0] = std::max(x[9], _lambda_min);
  l2._[1] = std::max(x[10], _lambda_min);
  l2._[2] = std::max(x[11], _lambda_min);

  // Third orientation.
  m3 = rotation_main_dir(x[12], x[13], x[14]);

  // Flip orientation if necessary.
  if (m3._[0] * old_m._[0] + m3._[1] * old_m._[1] + m3._[2] * old_m._[2] < 0) {
    m3 = -m3;
  }

  // Clamp lambdas.
  l3._[0] = std::max(x[15], _lambda_min);
  l3._[1] = std::max(x[16], _lambda_min);
  l3._[2] = std::max(x[17], _lambda_min);
}

// Functions for 1-tensor simple model.
void Simple1T::F(gmm::dense_matrix<double>& X) {
  assert(_signal_dim > 0);
  assert(gmm::mat_nrows(X) == static_cast<unsigned int>(_state_dim) &&
         gmm::mat_ncols(X) == static_cast<unsigned int>(2 * _state_dim + 1));

  for (size_t i = 0; i < gmm::mat_ncols(X); ++i) {
    // Normalize the direction vector.
    double norm_inv = 0.0; // 1e-16;
    norm_inv += X(0, i) * X(0, i);
    norm_inv += X(1, i) * X(1, i);
    norm_inv += X(2, i) * X(2, i);

    norm_inv = 1.0 / sqrt(norm_inv);
    X(0, i) *= norm_inv;
    X(1, i) *= norm_inv;
    X(2, i) *= norm_inv;

    // Clamp lambdas.
    X(3, i) = std::max(X(3, i), _lambda_min);
    X(4, i) = std::max(X(4, i), _lambda_min);
  }
}

void Simple1T::H(const gmm::dense_matrix<double>& X,
                 gmm::dense_matrix<double>& Y) {
  assert(_signal_dim > 0);
  assert(gmm::mat_nrows(X) == static_cast<unsigned int>(_state_dim) &&
         (gmm::mat_ncols(X) == static_cast<unsigned int>(2 * _state_dim + 1) ||
          gmm::mat_ncols(X) == 1));
  assert(gmm::mat_nrows(Y) == static_cast<unsigned int>(_signal_dim) &&
         (gmm::mat_ncols(Y) == static_cast<unsigned int>(2 * _state_dim + 1) ||
          gmm::mat_ncols(Y) == 1));
  assert(_signal_data);
  double b = _signal_data->GetAvgBValue();
  const std::vector<vec_t>& gradients = _signal_data->gradients();

  for (size_t i = 0; i < gmm::mat_ncols(X); ++i) {
    // Normalize direction.
    vec_t m = make_vec(X(0, i), X(1, i), X(2, i));
    m /= norm(m);

    // Clamp lambdas.
    double l1 = std::max(X(3, i), _lambda_min);
    double l2 = std::max(X(4, i), _lambda_min);
    
    // Flip if necessary.
    // Why is that???
    if (m._[0] < 0) {
      m = -m;
    }

    // Calculate diffusion matrix.
    mat_t D = diffusion(m, l1, l2);

    // Reconstruct signal.
    for (int j = 0; j < _signal_dim; ++j) {
      const vec_t& u = gradients[j];
      Y(j, i) = exp(-b * dot(u, D * u));
    }
  }
}

void Simple1T::State2Tensor(const State& x, vec_t& m, vec_t& l) {
  // Orientation.
  m = make_vec(x[0], x[1], x[2]);

  // TODO: Remove the 1e-10.
  double n = norm(m) + 1e-10;
  m /= n;

  // Clamp lambdas.
  l._[0] = std::max(x[3], _lambda_min);
  l._[1] = std::max(x[4], _lambda_min);
  l._[2] = l._[1]; 
}

// Functions for 2-tensor simple model.
void Simple2T::F(gmm::dense_matrix<double>& X) {
  assert(_signal_dim > 0);
  assert(gmm::mat_nrows(X) == static_cast<unsigned int>(_state_dim) &&
         gmm::mat_ncols(X) == static_cast<unsigned int>(2 * _state_dim + 1));

  // Clamp lambdas.
  for (size_t i = 0; i < gmm::mat_ncols(X); ++i) {
    // Normalize the direction vectors.
    double norm_inv = 0.0; // 1e-16;
    norm_inv += X(0, i) * X(0, i);
    norm_inv += X(1, i) * X(1, i);
    norm_inv += X(2, i) * X(2, i);

    norm_inv = 1.0 / sqrt(norm_inv);
    X(0, i) *= norm_inv;
    X(1, i) *= norm_inv;
    X(2, i) *= norm_inv;

    norm_inv = 0.0; // 1e-16;
    norm_inv += X(5, i) * X(5, i);
    norm_inv += X(6, i) * X(6, i);
    norm_inv += X(7, i) * X(7, i);

    norm_inv = 1.0 / sqrt(norm_inv);
    X(5, i) *= norm_inv;
    X(6, i) *= norm_inv;
    X(7, i) *= norm_inv;

    // Clamp lambdas.
    X(3, i) = std::max(X(3, i), _lambda_min);
    X(4, i) = std::max(X(4, i), _lambda_min);

    X(8, i) = std::max(X(8, i), _lambda_min);
    X(9, i) = std::max(X(9, i), _lambda_min);
  }
}

void Simple2T::H(const gmm::dense_matrix<double>& X,
                 gmm::dense_matrix<double>& Y) {
  assert(_signal_dim > 0);
  assert(gmm::mat_nrows(X) == static_cast<unsigned int>(_state_dim) &&
         (gmm::mat_ncols(X) == static_cast<unsigned int>(2 * _state_dim + 1) ||
          gmm::mat_ncols(X) == 1));
  assert(gmm::mat_nrows(Y) == static_cast<unsigned int>(_signal_dim) &&
         (gmm::mat_ncols(Y) == static_cast<unsigned int>(2 * _state_dim + 1) ||
          gmm::mat_ncols(Y) == 1));
  assert(_signal_data);
  double b = _signal_data->GetAvgBValue();
  const std::vector<vec_t>& gradients = _signal_data->gradients();

  for (size_t i = 0; i < gmm::mat_ncols(X); ++i) {
    // Normalize directions.
    vec_t m1 = make_vec(X(0, i), X(1, i), X(2, i));
    vec_t m2 = make_vec(X(5, i), X(6, i), X(7, i));
    m1 /= norm(m1);
    m2 /= norm(m2);

    // Clamp lambdas.
    double l11 = std::max(X(3, i), _lambda_min);
    double l12 = std::max(X(4, i), _lambda_min);
    
    double l21 = std::max(X(8, i), _lambda_min);
    double l22 = std::max(X(9, i), _lambda_min);

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
    for (int j = 0; j < _signal_dim; ++j) {
      const vec_t& u = gradients[j];
      Y(j, i) = (exp(-b * dot(u, D1 * u)) + exp(-b * dot(u, D2 * u))) / 2.0;
    }
  }
}

void Simple2T::State2Tensor(const State& x, const vec_t& old_m, vec_t& m1,
                          vec_t& l1, vec_t& m2, vec_t& l2) {
  // Orientations;
  m1 = make_vec(x[0], x[1], x[2]);
  m2 = make_vec(x[5], x[6], x[7]);
  m1 /= norm(m1);
  m2 /= norm(m2);

  // Clamp lambdas.
  l1._[0] = std::max(x[3], _lambda_min);
  l1._[1] = std::max(x[4], _lambda_min);
  l1._[2] = l1._[1]; 
  l2._[0] = std::max(x[8], _lambda_min);
  l2._[1] = std::max(x[9], _lambda_min);
  l2._[2] = l2._[1]; 

  // Flip orientations if necessary. (For m1 it should not happen, maybe for
  // m2.)
  if (m1._[0] * old_m._[0] + m1._[1] * old_m._[1] + m1._[2] * old_m._[2] < 0) {
    m1 = -m1;
  }
  if (m2._[0] * old_m._[0] + m2._[1] * old_m._[1] + m2._[2] * old_m._[2] < 0) {
    m2 = -m2;
  }
}

// Functions for 3-tensor simple model.
void Simple3T::F(gmm::dense_matrix<double>& X) {
  assert(_signal_dim > 0);
  assert(gmm::mat_nrows(X) == static_cast<unsigned int>(_state_dim) &&
         gmm::mat_ncols(X) == static_cast<unsigned int>(2 * _state_dim + 1));

  // Clamp lambdas.
  for (size_t i = 0; i < gmm::mat_ncols(X); ++i) {
    // Normalize the direction vectors.
    double norm_inv = 0.0; // 1e-16;
    norm_inv += X(0, i) * X(0, i);
    norm_inv += X(1, i) * X(1, i);
    norm_inv += X(2, i) * X(2, i);

    norm_inv = 1.0 / sqrt(norm_inv);
    X(0, i) *= norm_inv;
    X(1, i) *= norm_inv;
    X(2, i) *= norm_inv;

    norm_inv = 0.0; // 1e-16;
    norm_inv += X(5, i) * X(5, i);
    norm_inv += X(6, i) * X(6, i);
    norm_inv += X(7, i) * X(7, i);

    norm_inv = 1.0 / sqrt(norm_inv);
    X(5, i) *= norm_inv;
    X(6, i) *= norm_inv;
    X(7, i) *= norm_inv;

    norm_inv = 0.0; // 1e-16;
    norm_inv += X(10, i) * X(10, i);
    norm_inv += X(11, i) * X(11, i);
    norm_inv += X(12, i) * X(12, i);

    norm_inv = 1.0 / sqrt(norm_inv);
    X(10, i) *= norm_inv;
    X(11, i) *= norm_inv;
    X(12, i) *= norm_inv;

    // Clamp lambdas.
    X(3, i) = std::max(X(3, i), _lambda_min);
    X(4, i) = std::max(X(4, i), _lambda_min);

    X(8, i) = std::max(X(8, i), _lambda_min);
    X(9, i) = std::max(X(9, i), _lambda_min);

    X(13, i) = std::max(X(13, i), _lambda_min);
    X(14, i) = std::max(X(14, i), _lambda_min);
  }
}

void Simple3T::H(const gmm::dense_matrix<double>& X,
                 gmm::dense_matrix<double>& Y) {
  assert(_signal_dim > 0);
  assert(gmm::mat_nrows(X) == static_cast<unsigned int>(_state_dim) &&
         (gmm::mat_ncols(X) == static_cast<unsigned int>(2 * _state_dim + 1) ||
          gmm::mat_ncols(X) == 1));
  assert(gmm::mat_nrows(Y) == static_cast<unsigned int>(_signal_dim) &&
         (gmm::mat_ncols(Y) == static_cast<unsigned int>(2 * _state_dim + 1) ||
          gmm::mat_ncols(Y) == 1));
  assert(_signal_data);
  double b = _signal_data->GetAvgBValue();
  const std::vector<vec_t>& gradients = _signal_data->gradients();

  for (size_t i = 0; i < gmm::mat_ncols(X); ++i) {
    // Normalize directions.
    vec_t m1 = make_vec(X(0, i), X(1, i), X(2, i));
    vec_t m2 = make_vec(X(5, i), X(6, i), X(7, i));
    vec_t m3 = make_vec(X(10, i), X(11, i), X(12, i));
    m1 /= norm(m1);
    m2 /= norm(m2);
    m3 /= norm(m3);

    // Clamp lambdas.
    double l11 = std::max(X(3, i), _lambda_min);
    double l12 = std::max(X(4, i), _lambda_min);
    
    double l21 = std::max(X(8, i), _lambda_min);
    double l22 = std::max(X(9, i), _lambda_min);

    double l31 = std::max(X(13, i), _lambda_min);
    double l32 = std::max(X(14, i), _lambda_min);

    // flip if necessary
    if (m1._[0] < 0) {
      m1 = -m1;
    }
    if (m2._[0] < 0) {
      m2 = -m2;
    }
    if (m3._[0] < 0) {
      m3 = -m3;
    }

    // Calculate diffusion matrix.
    mat_t D1 = diffusion(m1, l11, l12);
    mat_t D2 = diffusion(m2, l21, l22);
    mat_t D3 = diffusion(m3, l31, l32);

    // Reconstruct signal.
    for (int j = 0; j < _signal_dim; ++j) {
      const vec_t& u = gradients[j];
      Y(j, i) = (exp(-b * dot(u, D1 * u)) +
                 exp(-b * dot(u, D2 * u)) +
                 exp(-b * dot(u, D3 * u))) / 2.0;
    }
  }
}

void Simple3T::State2Tensor(const State& x, const vec_t& old_m, vec_t& m1,
                            vec_t& l1, vec_t& m2, vec_t& l2, vec_t& m3,
                            vec_t& l3) {
  // Orientations;
  m1 = make_vec(x[0], x[1], x[2]);
  m2 = make_vec(x[5], x[6], x[7]);
  m3 = make_vec(x[10], x[11], x[12]);
  m1 /= norm(m1);
  m2 /= norm(m2);
  m3 /= norm(m3);

  // Clamp lambdas.
  l1._[0] = std::max(x[3], _lambda_min);
  l1._[1] = std::max(x[4], _lambda_min);
  l1._[2] = l1._[1]; 
  l2._[0] = std::max(x[8], _lambda_min);
  l2._[1] = std::max(x[9], _lambda_min);
  l2._[2] = l2._[1]; 
  l3._[0] = std::max(x[13], _lambda_min);
  l3._[1] = std::max(x[14], _lambda_min);
  l3._[2] = l3._[1]; 

  // Flip orientations if necessary. (For m1 it should not happen, maybe for
  // m2.)
  if (m1._[0] * old_m._[0] + m1._[1] * old_m._[1] + m1._[2] * old_m._[2] < 0) {
    m1 = -m1;
  }
  if (m2._[0] * old_m._[0] + m2._[1] * old_m._[1] + m2._[2] * old_m._[2] < 0) {
    m2 = -m2;
  }
  if (m3._[0] * old_m._[0] + m3._[1] * old_m._[1] + m3._[2] * old_m._[2] < 0) {
    m3 = -m3;
  }
}
