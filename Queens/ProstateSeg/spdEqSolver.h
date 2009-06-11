#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_sparse_matrix.h>

#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/algo/vnl_sparse_symmetric_eigensystem.h>


typedef double elementType;

typedef vnl_matrix<elementType> mtx;
typedef vnl_vector<elementType> vec;
typedef vnl_sparse_matrix<elementType> spMtx;


bool isPosDef(mtx* A);
bool isPosDef(spMtx* A);


int solveSpdEq(mtx* A, vec* b, vec* x, int numIter = -1, elementType tol = 1e-10);
int solveSpdEq(spMtx* A, vec* b, vec* x, int numIter = -1, elementType tol = 1e-10);
