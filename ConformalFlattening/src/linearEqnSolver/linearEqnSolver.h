#include <vcl_iostream.h>
#include <vnl/vnl_cost_function.h>
#include <vnl/vnl_math.h>

#include <vnl/algo/vnl_conjugate_gradient.h>

class theFunc : public vnl_cost_function {
 public:  
  theFunc(vnl_matrix<double> const& A, 
          vnl_vector<double> const& b);

  double f(vnl_vector<double> const& x);
  void gradf(vnl_vector<double> const& x, vnl_vector<double> & g);

  inline unsigned int dim() {return _dim;}

 private:
  vnl_matrix<double> const* _A;
  vnl_vector<double> const* _b;
  unsigned int _dim;
};


class linearEqnSolver {
 public:  
  linearEqnSolver(vnl_matrix<double> const& A, 
                  vnl_vector<double> const& b);

  vnl_vector<double> solve();

 private:
  theFunc _f;
  vnl_conjugate_gradient _cg;
};

