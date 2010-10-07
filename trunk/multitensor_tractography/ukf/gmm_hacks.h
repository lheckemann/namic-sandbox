#ifndef GMM_HACKS_H_
#define GMM_HACKS_H_

#include <cassert>
#include <gmm/gmm.h>


// Here we wrap some Lapack functions that we need but that don't have a gmm++
// wrapper already.
extern "C" {
  void dgesv_(...); 
  void dpotrf_(...);
}

inline void lu_solve_hack(const gmm::dense_matrix<double>& A,
                          gmm::dense_matrix<double>& X,
                          const gmm::dense_matrix<double>& B) {
  int n = static_cast<int>(gmm::mat_nrows(A));
  int nrhs = static_cast<int>(gmm::mat_ncols(B));
  const std::vector<int> ipvt(n);
  int info;
  gmm::copy(B, X);
  gmm::dense_matrix<double> A_copy(n, n);
  gmm::copy(A, A_copy);
  if (n) {
    dgesv_(&n, &nrhs, &(A_copy(0, 0)), &n, &ipvt[0], &(X(0, 0)), &n, &info);
    assert(info == 0);
  }
}

// Cholesky decomposition with double precision.
inline void cholesky_hack(const gmm::dense_matrix<double>& A,
                          gmm::dense_matrix<double>& L) {
  const char uplo = 'L';
  int n = static_cast<int>(gmm::mat_nrows(A));
  int info;
  gmm::copy(A, L);
  if (n) {
    dpotrf_(&uplo, &n, &(L(0, 0)), &n, &info);
    assert(info == 0);
  }

  // Set the rest of the upper triangular part of the matrix to zero.
  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {
      L(i, j) = 0.0;
    }
  }
}

#endif  // GMM_HACKS_H_
