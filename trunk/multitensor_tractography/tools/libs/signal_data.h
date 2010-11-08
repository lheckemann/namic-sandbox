#ifndef SIGNAL_DATA_H_
#define SIGNAL_DATA_H_

#include <string>
#include <vector>
#include <gmm/gmm.h>
#include <teem/nrrd.h>
#include "linalg.h"


// A class desribing DWI and label data. This is also the link to the teem nrrd
// library to load images.
class SignalData {
 public:
  SignalData(double sigma);
  virtual ~SignalData() { }

  // Gets the signal values at a specified position.
  virtual void Interp3Signal(const vec_t& pos,
                             std::vector<double>& signal) const= 0;

  // Checks if a certian position is still within the brain mask.
  virtual double Interp3ScalarMask(const vec_t& pos) const = 0;

  // Get all the seed points.
  virtual void GetSeeds(const std::vector<int>& labels,
                        std::vector<vec_t>& seeds) const = 0;

  // Returns the gradients.
  virtual const std::vector<vec_t>& gradients() const = 0;

  virtual double GetAvgBValue() const = 0;

  virtual int GetSignalDimension() const = 0;

  virtual bool LoadData(const std::string& data_file,
                        const std::string& seed_file,
                        const std::string& mask_file) = 0;

  virtual bool LoadSignal(const std::string& data_file) = 0;

  // Returns the voxel spacing.
  vec_t voxel() const {
    return _voxel;
  }

  // Returns the dimensions of the image.
  virtual vec_t dim() const = 0;

  const gmm::dense_matrix<double> i2r() const {
    return _i2r;
  }
  const gmm::dense_matrix<double> r2i() const {
    return _r2i;
  }

 protected:
  const double _sigma;

  vec_t _voxel;

  gmm::dense_matrix<double> _r2i;
  gmm::dense_matrix<double> _i2r;
};

// Class wrapping teem nrrd for loading nrrd files.
class NrrdData : public SignalData {
 public:
  NrrdData(double sigma);
  ~NrrdData();

  // Interpolates the DWI signal at a certain position.
  virtual void Interp3Signal(const vec_t& pos,
                             std::vector<double>& signal) const;
  virtual double Interp3ScalarMask(const vec_t& pos) const;

  // Gets the seed points from the nrrd file.
  virtual void GetSeeds(const std::vector<int>& labels,
                        std::vector<vec_t>& seeds) const;

  virtual const std::vector<vec_t>& gradients() const {
    return _gradients;
  }

  virtual double GetAvgBValue() const {
    double sum = 0.0;
    int size = _b_values.size();
    for (int i = 0; i < size; ++i) {
      sum += _b_values[i];
    }

    return sum / static_cast<double>(size);
  }

  virtual int GetSignalDimension() const {
    return _num_gradients;
  }

  virtual bool LoadData(const std::string& data_file,
                        const std::string& seed_file,
                        const std::string& mask_file);

  virtual bool LoadSignal(const std::string& data_file);

  virtual vec_t dim() const {
    return _dim;
  }

 private:
  vec_t _dim;

  int _num_gradients;
  std::vector<vec_t> _gradients;

  std::vector<double> _b_values;

  float *_data;
  void *_seed_data;
  int _seed_data_type;
  short *_mask_data;

  Nrrd *_data_nrrd;
  Nrrd *_seed_nrrd;
  Nrrd *_mask_nrrd;
};


#endif  // SIGNAL_DATA_H_
