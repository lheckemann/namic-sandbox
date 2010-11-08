#ifndef UNSCENTED_KALMAN_FILTER_H_
#define UNSCENTED_KALMAN_FILTER_H_

#include <vector>
#include <gmm/gmm.h>

class FilterModel;

typedef std::vector<double> State;

class UnscentedKalmanFilter {
 public:
  // The UKF is initialized with a filter model that defines the observation
  // and transition functions. Also required are the noise parameters.
  UnscentedKalmanFilter(FilterModel *filter_model);

  // Does the one filter step. Requires state, covariance and the signal and it
  // returns an updated state and an updated covariance.
  void Filter(const State& x, const gmm::dense_matrix<double>& p,
              const State& z, State& x_new, gmm::dense_matrix<double>& p_new);

 private:
  // Spreads the points around the current state using the covariance.
  void SigmaPoints(const State& x, const gmm::dense_matrix<double>& p,
                   gmm::dense_matrix<double>& x_spread);

  FilterModel *_filter_model;

  int _state_dim;
  double _scale;

  std::vector<double> _w;
  gmm::dense_matrix<double> _w_;

  double _k;
 
  // Temporary storage.
  gmm::dense_matrix<double> dim_dimext;
  gmm::dense_matrix<double> signaldim_dimext;
  gmm::dense_matrix<double> dim_dim;
  gmm::dense_matrix<double> dim_signaldim;
  gmm::dense_matrix<double> signaldim_dim;
  gmm::dense_matrix<double> X;
  std::vector<double> X_hat;
  gmm::dense_matrix<double> X_;
  gmm::dense_matrix<double> Y;
  std::vector<double> Y_hat;
  gmm::dense_matrix<double> Pxy;
  gmm::dense_matrix<double> Pyy;
  gmm::dense_matrix<double> K;
};

#endif  // UNSCENTED_KALMAN_FILTER_H_
