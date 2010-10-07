#ifndef FILTER_MODEL_H_
#define FILTER_MODEL_H_

#include <cassert>
#include <vector>
#include "linalg.h"
#include "signal_data.h"
#include "unscented_kalman_filter.h"


// A generic class that defines the transition function and the observation
// model for a Kalman filter.
struct FilterModel {
  FilterModel(int state_dim, double rs)
      : _state_dim(state_dim), _rs(rs), _signal_dim(0), _signal_data(NULL) {
    gmm::resize(_Q, _state_dim, _state_dim);

    // TODO: Fill in the real values.
    //_gradients.resize(_signal_dim);
  }

  virtual ~FilterModel() { }

  // Transition and observation functions. Each coloumn in the incoming matrix
  // represents one state.
  virtual void F(gmm::dense_matrix<double>& X) = 0;
  virtual void H(const gmm::dense_matrix<double>& X,
                 gmm::dense_matrix<double>& Y) = 0;

  // Functions that convert the state into a tensor representation that can be
  // used for tractography (meaning the main direction and all the eigenvalues/
  // lambdas).
  virtual void State2Tensor(const State& x, vec_t& m, vec_t& l) {
    assert(!"Not implemented");
  }
  virtual void State2Tensor(const State& x, const vec_t& old_m, vec_t& m1,
                            vec_t& l1, vec_t& m2, vec_t& l2) {
    assert(!"Not implemented");
  }
  virtual void State2Tensor(const State& x, const vec_t& old_m, vec_t& m1,
                            vec_t& l1, vec_t& m2, vec_t& l2, vec_t& m3,
                            vec_t& l3) {
    assert(!"Not implemented");
  }

  int state_dim() const {
    return _state_dim;
  }

  void set_signal_dim(int dim) {
    _signal_dim = dim;

    gmm::resize(_R, _signal_dim, _signal_dim);
    for (int i = 0; i < _signal_dim; ++i) {
      _R(i, i) = _rs;
    }
  }
  int signal_dim() const {
    return _signal_dim;
  }

  // The noise matrices used by the UKF.
  const gmm::dense_matrix<double>& Q() const {
    return _Q;
  }
  const gmm::dense_matrix<double>& R() const {
    return _R;
  }

  void set_signal_data(SignalData *signal_data) {
    _signal_data = signal_data;
  }

 protected:
  const int _state_dim;
  double _rs;
  int _signal_dim;

  SignalData *_signal_data;

  gmm::dense_matrix<double> _Q;
  gmm::dense_matrix<double> _R;
};

// Model describing 1-tensor tractography with the full tensor representation
// (3 angles and 3 eigenvalues).
struct Full1T : public FilterModel {
  Full1T(double qs, double ql, double rs)
      : FilterModel(6, rs), _lambda_min(100.0) { 
    _Q(0, 0) = _Q(1, 1) = _Q(2, 2) = qs;
    _Q(3, 3) = _Q(4, 4) = _Q(5, 5) = ql;
  }

  virtual ~Full1T() { }

  virtual void F(gmm::dense_matrix<double>& X);
  virtual void H(const gmm::dense_matrix<double>& X,
                 gmm::dense_matrix<double>& Y);

  virtual void State2Tensor(const State& x, vec_t& m, vec_t& l);

  const double _lambda_min;
};

// Model describing 2-tensor tractography with the full tensor representation
// (2 times 3 angles and 3 eigenvalues).
struct Full2T : public FilterModel {
  Full2T(double qs, double ql, double rs)
      : FilterModel(12, rs), _lambda_min(100.0) {
    _Q(0, 0) = _Q(1, 1) = _Q(2, 2) = _Q(6, 6) = _Q(7, 7) = _Q(8, 8) = qs;
    _Q(3, 3) = _Q(4, 4) = _Q(5, 5) = _Q(9, 9) = _Q(10, 10) = _Q(11, 11) = ql;
   }

  virtual ~Full2T() { }

  virtual void F(gmm::dense_matrix<double>& X);
  virtual void H(const gmm::dense_matrix<double>& X,
                 gmm::dense_matrix<double>& Y);

  virtual void State2Tensor(const State& x, const vec_t& old_m, vec_t& m1,
                            vec_t& l1, vec_t& m2, vec_t& l2);

  const double _lambda_min;
};

// Model describing 3-tensor tractography with the full tensor representation
// (3 times 3 angles and 3 eigenvalues).
struct Full3T : public FilterModel {
  Full3T(double qs, double ql, double rs)
      : FilterModel(18, rs), _lambda_min(100.0) {
    _Q(0, 0) = _Q(1, 1) = _Q(2, 2) = qs;
    _Q(6, 6) = _Q(7, 7) = _Q(8, 8) = qs;
    _Q(12, 12) = _Q(13, 13) = _Q(14, 14) = qs;

    _Q(3, 3) = _Q(4, 4) = _Q(5, 5) = ql;
    _Q(9, 9) = _Q(10, 10) = _Q(11, 11) = ql;
    _Q(15, 15) = _Q(16, 16) = _Q(17, 17) = ql;
   }

  virtual ~Full3T() { }

  virtual void F(gmm::dense_matrix<double>& X);
  virtual void H(const gmm::dense_matrix<double>& X,
                 gmm::dense_matrix<double>& Y);

  virtual void State2Tensor(const State& x, const vec_t& old_m, vec_t& m1,
                            vec_t& l1, vec_t& m2, vec_t& l2, vec_t& m3,
                            vec_t& l3);

  const double _lambda_min;
};

// Model describing 1-tensor tractography with the simple tensor representation
// (1 direction and 2 eigenvalues (2. and 3. eigenvalues are the same)).
struct Simple1T : public FilterModel {
  Simple1T(double qs, double ql, double rs)
      : FilterModel(5, rs), _lambda_min(100.0) {
    _Q(0, 0) = _Q(1, 1) = _Q(2, 2) = qs;
    _Q(3, 3) = _Q(4, 4) = ql;
  }

  virtual ~Simple1T() { }

  virtual void F(gmm::dense_matrix<double>& X);
  virtual void H(const gmm::dense_matrix<double>& X,
                 gmm::dense_matrix<double>& Y);

  virtual void State2Tensor(const State& x, vec_t& m, vec_t& l);

  const double _lambda_min;
};

// Model describing 2-tensor tractography with the simple tensor representation
// (2 times 1 direction and 2 eigenvalues (2. and 3. eigenvalues are the same)).
struct Simple2T : public FilterModel {
  Simple2T(double qs, double ql, double rs)
      : FilterModel(10, rs), _lambda_min(100.0) {
    _Q(0, 0) = _Q(1, 1) = _Q(2, 2) = _Q(5, 5) = _Q(6, 6) = _Q(7, 7) = qs;
    _Q(3, 3) = _Q(4, 4) = _Q(8, 8) = _Q(9, 9) = ql;
   }

  virtual ~Simple2T() { }

  virtual void F(gmm::dense_matrix<double>& X);
  virtual void H(const gmm::dense_matrix<double>& X,
                 gmm::dense_matrix<double>& Y);

  virtual void State2Tensor(const State& x, const vec_t& old_m, vec_t& m1,
                            vec_t& l1, vec_t& m2, vec_t& l2);

  const double _lambda_min;
};

// Model describing 3-tensor tractography with the simple tensor representation
// (3 times 1 direction and 2 eigenvalues (2. and 3. eigenvalues are the same)).
struct Simple3T : public FilterModel {
  Simple3T(double qs, double ql, double rs)
      : FilterModel(15, rs), _lambda_min(100.0) {
    _Q(0, 0) = _Q(1, 1) = _Q(2, 2) = qs;
    _Q(5, 5) = _Q(6, 6) = _Q(7, 7) = qs;
    _Q(10, 10) = _Q(11, 11) = _Q(12, 12) = qs;

    _Q(3, 3) = _Q(4, 4) = _Q(8, 8) = _Q(9, 9) = _Q(13, 13) = _Q(14, 14) = ql;
   }

  virtual ~Simple3T() { }

  virtual void F(gmm::dense_matrix<double>& X);
  virtual void H(const gmm::dense_matrix<double>& X,
                 gmm::dense_matrix<double>& Y);

  virtual void State2Tensor(const State& x, const vec_t& old_m, vec_t& m1,
                            vec_t& l1, vec_t& m2, vec_t& l2, vec_t& m3,
                            vec_t& l3);

  const double _lambda_min;
};

#endif  // FILTER_MODEL_H_
