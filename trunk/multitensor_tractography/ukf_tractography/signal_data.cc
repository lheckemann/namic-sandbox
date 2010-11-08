#include "signal_data.h"
#include <iostream>
#include <cassert>


SignalData::SignalData(double sigma)
    : _sigma(sigma) {

 }

NrrdData::NrrdData(double sigma)
    : SignalData(sigma), _data(NULL), _seed_data(NULL), _mask_data(NULL) {
 
 }

NrrdData::~NrrdData() {
  if (_data_nrrd) {
    nrrdNuke(_data_nrrd);
    if (_seed_data) {
      nrrdNuke(_seed_nrrd);
    }
    if (_mask_data) {
      nrrdNuke(_mask_nrrd);
    }
  }
}

void NrrdData::Interp3Signal(const vec_t& pos,
                             std::vector<double>& signal) const {
  const int nx = _dim._[0];
  const int ny = _dim._[1];
  const int nz = _dim._[2];

  double w_sum = 1e-16;

  assert(signal.size() == static_cast<size_t>(_num_gradients * 2));
  assert(_data);

  for (int i = 0; i < 2 * _num_gradients; ++i) {
    signal[i] = 0.0;
  }

  int step1 = nz * ny * _num_gradients;
  int step2 = nz * _num_gradients;

  for (int xx = -1; xx <= 1; ++xx) {
    int x = round(pos._[0]) + xx;
    if (x < 0 || nx <= x) {
      continue;
    }
    double dx = (x - pos._[0]) * _voxel._[0];
    double dxx = dx * dx;

    for (int yy = -1; yy <= 1; ++yy) {
      int y = round(pos._[1]) + yy;
      if (y < 0 || ny <= y) {
        continue;
      }

      double dy = (y - pos._[1]) * _voxel._[1];
      double dyy = dy * dy;

      for (int zz = -1; zz <= 1; ++zz) {
        int z = round(pos._[2]) + zz;
        if (z < 0 || nz <= z) {
          continue;
        }
        double dz = (z - pos._[2]) * _voxel._[2];
        double dzz = dz * dz;

        double w = exp(-(dxx + dyy + dzz) / _sigma);

        for (int i = 0; i < _num_gradients; ++i) {
          signal[i] += w * _data[step1 * x + step2 * y + z * _num_gradients + i];
        }

        w_sum += w;
      }
    }
  }

  w_sum *= 2; // Double each occurance.
  for (int i = 0; i < _num_gradients; ++i) {
    signal[i] /= w_sum;

    // Push into second spot.
    signal[i + _num_gradients]  = signal[i];
  }

}

double NrrdData::Interp3ScalarMask(const vec_t& pos) const {
  const int nx = _dim._[0];
  const int ny = _dim._[1];
  const int nz = _dim._[2];

  double w_sum = 1e-16;
  double s = 0.0;

  for (int xx = -1; xx <= 1; xx++) {
    int x = round(pos._[0]) + xx;
    if (x < 0 || nx <= x) {
      continue;
    }
    double dx = (x - pos._[0]) * _voxel._[0];
    double dxx = dx * dx;

    for (int yy = -1; yy <= 1; yy++) {
      int y = round(pos._[1]) + yy;
      if (y < 0 || ny <= y) {
        continue;
      }
      double dy = (y - pos._[1]) * _voxel._[1];
      double dyy = dy * dy;

      for (int zz = -1; zz <= 1; zz++) {
        int z = round(pos._[2]) + zz;
        if (z < 0 || nz <= z) {
          continue;
        }
        double dz = (z - pos._[2]) * _voxel._[2];
        double dzz = dz * dz;

        double w = exp(-(dxx + dyy + dzz) / _sigma);
        if (_mask_data[nz * ny * x + nz * y + z]) {
          s += w;
        }

        w_sum += w;
      }
    }
  }

  return s / w_sum;
}

void NrrdData::GetSeeds(const std::vector<int>& labels,
                        std::vector<vec_t>& seeds) const {
  if (_seed_data) {
    assert(seeds.size() == 0);
    std::vector<int>::const_iterator cit;

    // Go through the volume.
    int nx = _seed_nrrd->axis[2].size;
    int ny = _seed_nrrd->axis[1].size;
    int nz = _seed_nrrd->axis[0].size;
    assert(_seed_data);
    assert(nx == _dim._[0] && ny == _dim._[1] && nz == _dim._[2]);
    for (int i = 0; i < nx; ++i) {
      for (int j = 0; j < ny; ++j) {
        for (int k = 0; k < nz; ++k) {
          for (cit = labels.begin(); cit != labels.end(); ++cit) {
            int value = 0;
            int index = ny * nz * i + nz * j + k;
            switch (_seed_data_type) {
            case 2:
              value = static_cast<unsigned char *>(_seed_data)[index];
              break;
            case 3:
              value = static_cast<short *>(_seed_data)[index];
              break;
            case 5:
              value = static_cast<int *>(_seed_data)[index];
              break;
            default:
              std::cout << "Unsupported data type for seed file!" << std::endl;
              assert(false);
            }
            if (*cit == value) {
              seeds.push_back(make_vec(i, j, k));
            }
          }
        }
      }
    }
  } else {
    std::cout << "No seed data available." << std::endl;
  }
}

bool NrrdData::LoadData(const std::string& data_file,
                        const std::string& seed_file,
                        const std::string& mask_file) {
  if (_data || _seed_data || _mask_data) {
    std::cout << "There is already some data!" << std::endl;
    return true;
  }

  if (LoadSignal(data_file)) {
    return true;
  }

  _mask_nrrd = nrrdNew();

  char *err;
  if (!seed_file.empty()) {
    _seed_nrrd = nrrdNew();
    if (nrrdLoad(_seed_nrrd, seed_file.c_str(), NULL)) {
      err = biffGetDone(NRRD);
      std::cout << "Trouble reading " << seed_file << ": " << err << std::endl;
      delete err;
      return true;
    }

    _seed_data_type = _seed_nrrd->type;
    assert(_seed_data_type == 2 || _seed_data_type == 3 || _seed_data_type == 5);
    _seed_data = _seed_nrrd->data;
  }
  if (nrrdLoad(_mask_nrrd, mask_file.c_str(), NULL)) {
    err = biffGetDone(NRRD);
    std::cout << "Trouble reading " << mask_file << ": " << err << std::endl;
    delete err;
    return true;
  }

  // We might have to extend this later on to support more file formats.
  assert(_mask_nrrd->type == 3);

  _mask_data = static_cast<short *>(_mask_nrrd->data);

  return false;
}

bool NrrdData::LoadSignal(const std::string& data_file) {
  _data_nrrd = nrrdNew();

  char *err;
  if (nrrdLoad(_data_nrrd, data_file.c_str(), NULL)) {
    err = biffGetDone(NRRD);
    std::cout << "Trouble reading " << data_file << ": " << err << std::endl;
    delete err;
    return true;
  }

  // We might have to extend this later on to support more file formats.
  assert(_data_nrrd->type == 9);
  _data = static_cast<float *>(_data_nrrd->data);

  int len = _data_nrrd->kvpArr->len;
  int size = _data_nrrd->kvpArr->size;

  if (size != 2) {
    size = 2;
  }
  assert(size == 2);

  double b = 0.0;

  assert(_gradients.size() == 0);

  // Read key value pairs.
  for (int i = 0; i < len * size; i += size) {
    std::string key(_data_nrrd->kvp[i]);

    if (!key.compare("DWMRI_b-value")) {
      b = atof(_data_nrrd->kvp[i + 1]);
    } else if (key.length() > 14 &&
               !key.substr(0, 14).compare("DWMRI_gradient")) {
      std::string val(_data_nrrd->kvp[i + 1]);
      double gx, gy, gz;
      int pos_first = val.find_first_of(" ");
      int pos_last = val.find_last_of(" ");
      gx = atof(val.substr(0, pos_first).c_str());
      gy = atof(val.substr(pos_first + 1, pos_last - pos_first).c_str());
      gz = atof(val.substr(pos_last + 1).c_str());

      _gradients.push_back(make_vec(gx, gy, gz));
    } else if (!key.compare("modality")) {
      assert(!std::string(_data_nrrd->kvp[i + 1]).compare("DWMRI"));
    }
  }

  // Get B values;
  double max = 0.0;
  for (size_t i = 0; i < _gradients.size(); ++i) {
    double sum = 0.0;
    for (int j = 0; j < 3; ++j) {
      sum += _gradients[i]._[j] * _gradients[i]._[j];
    }
    sum = sqrt(sum);
    if (sum > max) {
      max = sum;
    }
    _b_values.push_back(b * sum);
  }

  // HACK. I don't understand why this is needed.
  double max_inv = 1.0 / max;
  for (size_t i = 0; i < _gradients.size(); ++i) {
    _b_values[i] *= max_inv;
  }

  // Voxel spacing.
  double space_dir[NRRD_SPACE_DIM_MAX];
  double spacing1, spacing2, spacing3;
  nrrdSpacingCalculate(this->_data_nrrd, 1, &spacing1, space_dir);
  nrrdSpacingCalculate(this->_data_nrrd, 2, &spacing2, space_dir);
  nrrdSpacingCalculate(this->_data_nrrd, 3, &spacing3, space_dir);
  _voxel = make_vec(spacing3, spacing2, spacing1);

  // Dimensions.
  _dim = make_vec(_data_nrrd->axis[3].size, _data_nrrd->axis[2].size,
                  _data_nrrd->axis[1].size);

  _num_gradients = _data_nrrd->axis[0].size;
  assert(_num_gradients == static_cast<int>(_gradients.size()));

  // Measurement frame.
  gmm::dense_matrix<double> measurement_frame(3, 3);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      measurement_frame(i, j) = _data_nrrd->measurementFrame[i][j];
    }
  }

  // R2I.
  gmm::resize(_i2r, 4, 4);
  _i2r(3, 3) = 1.0;
  // TODO: Inhance this, this is probably too simple.
  _i2r(0, 0) = _data_nrrd->axis[1].spaceDirection[0];
  _i2r(1, 1) = _data_nrrd->axis[2].spaceDirection[1];
  _i2r(2, 2) = _data_nrrd->axis[3].spaceDirection[2];
  _i2r(0, 3) = _data_nrrd->spaceOrigin[0];
  _i2r(1, 3) = _data_nrrd->spaceOrigin[1];
  _i2r(2, 3) = _data_nrrd->spaceOrigin[2];

  // I2R.
  gmm::resize(_r2i, 4, 4);
  gmm::copy(_i2r, _r2i);
  gmm::lu_inverse(_r2i);

  // Transform gradients.
  gmm::dense_matrix<double> R(4, 4);
  gmm::copy(_r2i, R);
  gmm::resize(R, 3, 3);
  double vox_x_inv = 1.0 / _voxel._[0];
  double vox_y_inv = 1.0 / _voxel._[1];
  double vox_z_inv = 1.0 / _voxel._[2];
  R(0, 0) *=  vox_x_inv;
  R(0, 1) *=  vox_y_inv;
  R(0, 2) *=  vox_z_inv;
  R(1, 0) *=  vox_x_inv;
  R(1, 1) *=  vox_y_inv;
  R(1, 2) *=  vox_z_inv;
  R(2, 0) *=  vox_x_inv;
  R(2, 1) *=  vox_y_inv;
  R(2, 2) *=  vox_z_inv;
  gmm::lu_inverse(R);
  gmm::dense_matrix<double> tmp_mat(3, 3);
  gmm::mult(R, measurement_frame, tmp_mat);
  int count = _gradients.size();
  std::vector<double> u(3), u_new(3);
  for (int i = 0; i < count; ++i) {
    // Transform and normalize.
    u[0] = _gradients[i]._[0];
    u[1] = _gradients[i]._[1];
    u[2] = _gradients[i]._[2];
    gmm::mult(tmp_mat, u, u_new);

    double norm_inv = 1.0 / sqrt(u_new[0] * u_new[0] +
                                 u_new[1] * u_new[1] +
                                 u_new[2] * u_new[2]);
    _gradients[i]._[0] = u_new[0] * norm_inv;
    _gradients[i]._[1] = u_new[1] * norm_inv;
    _gradients[i]._[2] = u_new[2] * norm_inv;
  }
  for (int i = 0; i < count; ++i) {
    // Duplicate and reverse direction.
    _gradients.push_back(-_gradients[i]);
  }

  return false;
}
