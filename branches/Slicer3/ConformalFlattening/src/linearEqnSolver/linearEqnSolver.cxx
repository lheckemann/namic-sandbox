// .Description:
//
// This class defines a solver for the linear system Ax=b.It uses the
// Conjugate Gradient method. So the matrix A must be symmetric and
// positive defined. In many cases of the numerical computation for
// the solution of partial differential equations, those properties of
// A hold.

// .Mechanism
// However, the symmetry and positive define properties are not
// checked within this but left for the user. Basicall, this class
// optimizes the function y=\frac{1}{2}(x^T)*A*x - (b^T)*x.

// The above function is defined by the class theFunc which is derived
// from the vnl_cost_function.

// .How to use
// To use this class solve Ax=b:
// 1. define matrix A to be a vnl_matrix<double>
// 2. define vector b to be a vnl_vector<double>
// 3. linearEqnSolver les(A, b);
// 4. vnl_vector x = les.solve();


#include "linearEqnSolver.h"
#include <assert.h>

////////////////////////////////////////////////////////////////////
// implementation of class theFunc
theFunc::theFunc(vnl_matrix<double> const& A, 
                 vnl_vector<double> const& b)  
  :_A(&A), _b(&b), _dim(b.size()), vnl_cost_function(b.size()), _sparse(false) {

  if (A.rows() != b.size())
        assert(!"The # of rows in A must be the same as the length of b!");    
}

// overload construction function for sparse matrix A
theFunc::theFunc(vnl_sparse_matrix<double> const& A, 
                 vnl_vector<double> const& b)  
  :_Asparse(&A), _b(&b), _dim(b.size()), vnl_cost_function(b.size()), _sparse(true) {

  if (A.rows() != b.size())
        assert(!"The # of rows in A must be the same as the length of b!");    
}



double theFunc::f(vnl_vector<double> const& x) {  
  double r;
  if (_sparse == false) {
    r = 0.5*inner_product(x*(*_A),x)-inner_product((*_b),x);
  }
  else if (_sparse == true) {
    vnl_vector<double> tmp;
    _Asparse -> pre_mult(x, tmp);
     r = 0.5*inner_product(tmp,x)-inner_product((*_b),x);
  }

  return r;
}


void theFunc::gradf(vnl_vector<double> const& x, 
                            vnl_vector<double> & g) {
  if (_sparse == false) {
    g = (*_A)*x - (*_b);
  }
  else if (_sparse == true) {
    vnl_vector<double> tmp;
    _Asparse -> mult(x, tmp);
    g = tmp - (*_b);
  }
}

// implementation of class theFunc
////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////
// implementation of class linearEqnSolver

linearEqnSolver::linearEqnSolver(vnl_matrix<double> const& A, 
                                 vnl_vector<double> const& b)
  :_f(A, b), _cg(_f) {}

// overload construction function for sparse matrix
linearEqnSolver::linearEqnSolver(vnl_sparse_matrix<double> const& A, 
                                 vnl_vector<double> const& b)
  :_f(A, b), _cg(_f) {}


vnl_vector<double> linearEqnSolver::solve() {
  vnl_vector<double> x(_f.dim(), 0);
  _cg.minimize(x);
  return x;
}
// implementation of class linearEqnSolver
////////////////////////////////////////////////////////////////////

