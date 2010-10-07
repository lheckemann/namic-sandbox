#ifndef TRACTOGRAPHY_H_
#define TRACTOGRAPHY_H_

#include <string>
#include "linalg.h"
#include "unscented_kalman_filter.h"


class SignalData;

// This class performs the tractography and saves each step.
class Tractography {
 public:
  // A data structure for storing the fibers and their data.
  struct Fiber {
    std::vector<vec_t> position;
    std::vector<double> fa;
    std::vector<double> norm;
    std::vector<std::vector<double> > state;
    std::vector<gmm::dense_matrix<double> > covariance;
  };

  // Stores all information for a seed point to start tractography. The
  // start_dir is only used for the simple model and the angles only for the
  // complex/full representation.
  struct SeedPointInfo {
    State state;
    gmm::dense_matrix<double> covariance;
    vec_t point;
    vec_t start_dir;
    double fa;
  };

  // The constructor makes sure that all the parameter variables are set to a
  // correct/meaningful value.
  Tractography(FilterModel *model, const std::string& output_file,
               int num_tensors, bool is_full_model, double fa_min,
               double ga_min, int seeds, double theta_max,
               bool transform_position);

  ~Tractography();

  // Load the files that contain the DWI signal, the seeds and a mask
  // defining the volume of the brain.
  bool LoadFiles(const std::string& data_file, const std::string& seed_file,
                 const std::string& mask_file);

  // Creates the seeds and initilizes them by finding the tensor directions,
  // eigenvalues and Euler Angles. This also sets the initial state and 
  // covariance.
  void Init(std::vector<SeedPointInfo>& seed_infos);

  // Performs the tractography.
  void Run();

  void AddLabel(int label) {
    _labels.push_back(label);
  }

  void RecordFa() {
    _record_fa = true;
  }
  void RecordState() {
    _record_state = true;
  }
  void RecordCov() {
    _record_cov = true;
  }

 private:
  // Calculate six tensor coefficients by solving B * d = log(s), where d are
  // tensor coefficients, B is gradient weighting, s is signal.
  void UnpackTensor(double b, const std::vector<vec_t>& u,
                    std::vector<std::vector<double> >& s,
                    std::vector<std::vector<double> >& ret);

  // Performs the tractography. Follows one seed point. We use different
  // functions depending if we have one or two tensors.
  void Follow3T(const SeedPointInfo& seed, Fiber& fiber, bool is_branching,
                std::vector<SeedPointInfo>& branching_seeds);
  void Follow2T(const SeedPointInfo& seed, Fiber& fiber, bool is_branching,
                std::vector<SeedPointInfo>& branching_seeds);
  void Follow1T(const SeedPointInfo& seed, Fiber& fiber);

  // Does one step along the fiber.
  void Step3T(vec_t& x, vec_t& m1, vec_t& l1, vec_t& m2, vec_t& l2, vec_t& m3,
              vec_t& l3, double& fa, State& state,
              gmm::dense_matrix<double>& covariance);
  void Step2T(vec_t& x, vec_t& m1, vec_t& l1, vec_t& m2, vec_t& l2, double& fa,
              State& state, gmm::dense_matrix<double>& covariance);
  void Step1T(vec_t& x, double& fa, State& state,
              gmm::dense_matrix<double>& covariance);

  // Swaps the first tensor with the i-th tensor.
  void SwapState3T(State& state, gmm::dense_matrix<double>& covariance, int i);
  void SwapState2T(State& state, gmm::dense_matrix<double>& covariance);

  // Writes debug information about seeds to stdout.
  void PrintSeedInfo(const std::vector<SeedPointInfo>& seed_infos);

  // Saves one point along the fiber so that everything can be written to a
  // file at the end.
  void Record(const vec_t& x, double fa, const State& state,
              const gmm::dense_matrix<double> p, Fiber& fiber);

  UnscentedKalmanFilter *_ukf;

  SignalData *_signal_data;
  FilterModel *_model;

  std::vector<int> _labels;

  std::string _output_file;

  bool _record_fa;
  bool _record_state;
  bool _record_cov;

  const int _num_tensors;
  bool _is_full_model;

  bool _is_branching;

  // Parameters for the tractography.
  double _fa_min;
  double _ga_min;
  int _seeds_per_voxel;
  double _cos_theta_min;
  double _cos_theta_max;
  const double _p0;
  const double _sigma;
  double _dt;

  const double _min_radius;
  const int _max_length;

  std::vector<Fiber> _fibers;

  bool _full_brain;
  const double _full_brain_ga_min;
  
  bool _transform_position;
};

#endif  // TRACTOGRAPHY_H_
