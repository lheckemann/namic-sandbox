#include "unscented_kalman_filter.h"
#include <cassert>
#include "filter_model.h"
#include "gmm_hacks.h"


UnscentedKalmanFilter::UnscentedKalmanFilter(FilterModel *filter_model)
    : _filter_model(filter_model), _k(0.01) {
  int dim = _filter_model->state_dim();
  _scale = sqrt(dim + _k);

  _w.push_back(_k / (dim + _k));
  _w.insert(_w.end(), dim * 2, 0.5 / (dim + _k));

  // Create diagonal matrix.
  gmm::resize(_w_, 2 * dim + 1, 2 * dim + 1);
  for (int i = 0; i < 2 * dim + 1; ++i) {
    _w_(i, i) = _w[i];
  }

  assert(static_cast<int>(gmm::mat_nrows(_filter_model->Q())) == dim && 
         static_cast<int>(gmm::mat_ncols(_filter_model->Q())) == dim);

  int signal_dim = _filter_model->signal_dim();
  assert(static_cast<int>(gmm::mat_nrows(_filter_model->R())) == signal_dim && 
         static_cast<int>(gmm::mat_ncols(_filter_model->R())) == signal_dim);

  // Resizing of temporary storage.
  gmm::resize(dim_dimext, dim, 2 * dim + 1);
  gmm::resize(signaldim_dimext, signal_dim, 2 * dim + 1);
  gmm::resize(dim_dim, dim, dim);
  gmm::resize(dim_signaldim, dim, signal_dim);
  gmm::resize(signaldim_dim, signal_dim, dim);
  gmm::resize(X, dim, 2 * dim + 1);
  X_hat.resize(dim);
  gmm::resize(X_, dim, 2 * dim + 1);
  gmm::resize(Y, signal_dim, 2 * dim + 1);
  Y_hat.resize(signal_dim);
  gmm::resize(Pxy, dim, signal_dim);
  gmm::resize(Pyy, signal_dim, signal_dim);
  gmm::resize(K, signal_dim, dim);
}

void UnscentedKalmanFilter::SigmaPoints(const State& x,
                                        const gmm::dense_matrix<double>& p,
                                        gmm::dense_matrix<double>& x_spread) {
  int dim = _filter_model->state_dim();

  assert(gmm::mat_nrows(x_spread) == static_cast<unsigned int>(dim) &&
         gmm::mat_ncols(x_spread) == static_cast<unsigned int>(2 * dim + 1));
  assert(x.size() == static_cast<unsigned int>(dim));
  assert(gmm::mat_nrows(p) == static_cast<unsigned int>(dim) &&
         gmm::mat_ncols(p) == static_cast<unsigned int>(dim));

  gmm::dense_matrix<double> M(dim, dim);
  cholesky_hack(p, M);
  gmm::scale(M, _scale);

  // Horizontally stack x to the X matrix.
  gmm::dense_matrix<double> X(dim, dim);
  for (size_t i = 0; i < gmm::mat_ncols(X); ++i) {
    gmm::copy(x, gmm::mat_col(X, i));
  }
  
  // Create dim x (2 * dim + 1) matrix (x, x + m, x - m).
  gmm::copy(x, gmm::mat_col(x_spread, 0));
  gmm::add(X, M, gmm::sub_matrix(x_spread, gmm::sub_interval(0, dim),
                                 gmm::sub_interval(1, dim)));
  gmm::add(X, gmm::scaled(M, -1.0),
           gmm::sub_matrix(x_spread, gmm::sub_interval(0, dim),
                           gmm::sub_interval(dim + 1, dim)));
}

void UnscentedKalmanFilter::Filter(const State& x,
                                   const gmm::dense_matrix<double>& p,
                                   const State& z, State& x_new,
                                   gmm::dense_matrix<double>& p_new) {
  assert(static_cast<int>(x.size()) == _filter_model->state_dim());
  assert(static_cast<int>(z.size()) == _filter_model->signal_dim());
  assert(static_cast<int>(x_new.size()) == _filter_model->state_dim());
  assert(static_cast<int>(gmm::mat_nrows(p)) == _filter_model->state_dim() &&
         static_cast<int>(gmm::mat_ncols(p)) == _filter_model->state_dim());
  assert(static_cast<int>(gmm::mat_nrows(p_new)) == _filter_model->state_dim() && 
         static_cast<int>(gmm::mat_ncols(p_new)) == _filter_model->state_dim());
  assert(_filter_model);
  assert(static_cast<int>(_w.size()) == 2 * _filter_model->state_dim()+ 1);

  const gmm::dense_matrix<double>& Q = _filter_model->Q();
  const gmm::dense_matrix<double>& R = _filter_model->R();

  // Create sigma points.
  SigmaPoints(x, p, X);

  // Predict state (also its mean and covariance).
  _filter_model->F(X);
  gmm::mult(X, _w, X_hat);
  for (size_t i = 0; i < gmm::mat_ncols(dim_dimext); ++i) {
    gmm::copy(X_hat, gmm::mat_col(dim_dimext, i));
  }
  gmm::add(X, gmm::scaled(dim_dimext, -1.0), X_);
  gmm::mult(X_, _w_, dim_dimext);
  gmm::mult(dim_dimext, gmm::transposed(X_), p_new);
  gmm::add(Q, p_new);

  // Predict observation (also its mean and covariance).
  _filter_model->H(X, Y);
  gmm::mult(Y, _w, Y_hat);
  for (size_t i = 0; i < gmm::mat_ncols(signaldim_dimext); ++i) {
    gmm::copy(Y_hat, gmm::mat_col(signaldim_dimext, i));
  }
  gmm::add(gmm::scaled(signaldim_dimext, -1.0), Y);
  gmm::dense_matrix<double>& Y_ = Y;
  gmm::mult(Y_, _w_, signaldim_dimext);
  gmm::mult(signaldim_dimext, gmm::transposed(Y_), Pyy);
  gmm::add(R, Pyy);

  // Predict cross-correlation between state and observation.
  gmm::mult(X_, _w_, dim_dimext);
  gmm::mult(dim_dimext, gmm::transposed(Y_), Pxy);

  // Kalman gain K, estimate state/observation, compute covariance.
  gmm::copy(gmm::transposed(Pxy), signaldim_dim);
  lu_solve_hack(Pyy, K, signaldim_dim);
  gmm::mult(gmm::transposed(K), Pyy, dim_signaldim);
  gmm::mult(dim_signaldim, K, dim_dim);

  gmm::add(gmm::scaled(dim_dim, -1.0), p_new);
  gmm::scale(Y_hat, -1.0);
  gmm::add(z, Y_hat);
  gmm::mult(gmm::transposed(K), Y_hat, X_hat, x_new);
}
