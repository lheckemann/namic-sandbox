#ifndef shapeBasedSegCost_h_
#define shapeBasedSegCost_h_

#include "shapeBasedSeg.h"
#include <vnl/vnl_cost_function.h>

class CShapeBasedSegCost : public vnl_cost_function 
{
public:
  CShapeBasedSegCost( CShapeBasedSeg* const shapeBasedSeg );

  /*   double f(vnl_vector<double> const& x); */
  /*   void gradf (vnl_vector< double > const &x, vnl_vector< double > &gradient); */
  void compute(vnl_vector< double > const &x, double* f, vnl_vector< double >* g);


 private:

  CShapeBasedSeg* m_shapeBasedSeg;
};

#endif

