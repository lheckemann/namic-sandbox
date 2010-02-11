#ifndef ICPCost3_h_
#define ICPCost3_h_

#include "ICP3.h"
#include <vnl/vnl_cost_function.h>

class CICPCost3 : public vnl_cost_function 
{
 public:  
  CICPCost3( ICP3 *icp);

  /*   double f(vnl_vector<double> const& x); */
/*   void gradf (vnl_vector< double > const &x, vnl_vector< double > &gradient); */
  void compute(vnl_vector< double > const &x, double *f, vnl_vector< double > *g);


 private:

  ICP3 *m_ICP3;
};


#endif

