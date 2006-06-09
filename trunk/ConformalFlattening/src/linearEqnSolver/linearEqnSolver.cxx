#include "linearEqnSolver.h"
#include <assert.h>

////////////////////////////////////////////////////////////////////
// implementation of class theFunc
theFunc::theFunc(vnl_matrix<double> const& A, 
                 vnl_vector<double> const& b)  
  :_A(&A), _b(&b), _dim(b.size()), vnl_cost_function(b.size()) {

  if (A.rows() != b.size())
        assert(!"The # of rows in A must be the same as the length of b!");    
}


double theFunc::f(vnl_vector<double> const& x) {  
  double r = 0.5*inner_product(x*(*_A),x)-inner_product((*_b),x);
  return r;
}


void theFunc::gradf(vnl_vector<double> const& x, 
                            vnl_vector<double> & g) {
  g = (*_A)*x - (*_b);
}

// implementation of class theFunc
////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////
// implementation of class linearEqnSolver

linearEqnSolver::linearEqnSolver(vnl_matrix<double> const& A, 
                                 vnl_vector<double> const& b)
  :_f(A, b), _cg(_f) {}


vnl_vector<double> linearEqnSolver::solve() {
  vnl_vector<double> x(_f.dim(), 0);
  _cg.minimize(x);
  return x;
}
// implementation of class linearEqnSolver
////////////////////////////////////////////////////////////////////

