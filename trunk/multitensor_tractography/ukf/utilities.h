#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <cassert>
#include <cmath>
#include <vector>
#include <gmm/gmm.h>
#include "linalg.h"


// Simple functions (fa, ga) are defined here.

// Fractional anisotropy.
double l2fa(double l1, double l2, double l3) {
  if (l2 == l3) {
    // TODO: Add the / sqrt(3.0). This was a bug that was already there in the
    // Python module. It is not too bad since it's only a scaling problem.
    return fabs(l1 - l2) / sqrt(l1 * l1 + 2.0 * l2 * l2); // / sqrt(3.0);
  } else {
    double s = (l1 + l2 + l3) / 3.0;
    return sqrt(0.5 * ((l1 - s) * (l1 - s) +
                       (l2 - s) * (l2 - s) +
                       (l3 - s) * (l3 - s)) /
                (l1 * l1 + l2 * l2 + l3 * l3)); 
  }
}

// Generalized anisotropy.
double s2ga(const gmm::dense_matrix<double>& signal) {
  int n = gmm::mat_nrows(signal);
  assert(gmm::mat_ncols(signal) == 1);

  double mu = 0.0;
  double mu_sq = 0.0;

  for (int i = 0; i < n; ++i) {
    mu += signal(i, 0);
    mu_sq += signal(i, 0) * signal(i, 0);
  }

  double n_inv = 1.0 / n;

  mu *= n_inv;
  mu_sq *= n_inv;

  return sqrt(mu_sq - mu * mu) / sqrt(mu_sq);
}

double curve_radius(const std::vector<vec_t>& fiber) {
  int length = fiber.size();
  if (length < 3) {
    return 1.0;
  }

  vec_t v1 = fiber[length - 2] - fiber[length - 3];
  vec_t v2 = fiber[length - 1] - fiber[length - 2];

  // Normalize
  double n1 = norm(v1);
  double n2 = norm(v2);
  v1 /= n1;
  v2 /= n2;

  double curv = norm(2 * (v2 - v1) / (n2 + n1));
  if (std::isnan(curv)) {
    return 1.0;
  }

  return 1.0 / curv;
}

#endif  // UTILITIES_H_
