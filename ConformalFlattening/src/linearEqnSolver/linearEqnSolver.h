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


#include <vcl_iostream.h>
#include <vnl/vnl_cost_function.h>
#include <vnl/vnl_math.h>

#include <vnl/vnl_sparse_matrix.h>

#include <vnl/algo/vnl_conjugate_gradient.h>

class theFunc : public vnl_cost_function {
 public:  
  theFunc(vnl_matrix<double> const& A, 
          vnl_vector<double> const& b);

  theFunc(vnl_sparse_matrix<double> const& A, 
          vnl_vector<double> const& b);

  double f(vnl_vector<double> const& x);
  void gradf(vnl_vector<double> const& x, vnl_vector<double> & g);

  inline unsigned int dim() {return _dim;}

 private:
  vnl_matrix<double> const* _A;
  vnl_sparse_matrix<double> const* _Asparse;
  vnl_vector<double> const* _b;
  unsigned int _dim;
};


class linearEqnSolver {
 public:  
  linearEqnSolver(vnl_matrix<double> const& A, 
                  vnl_vector<double> const& b);

  linearEqnSolver(vnl_sparse_matrix<double> const& A, 
                  vnl_vector<double> const& b);

  vnl_vector<double> solve();

 private:
  theFunc _f;
  vnl_conjugate_gradient _cg;
  bool _sparse;
};

