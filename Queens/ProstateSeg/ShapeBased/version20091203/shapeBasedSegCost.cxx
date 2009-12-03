#include "shapeBasedSegCost.h"


CShapeBasedSegCost::CShapeBasedSegCost( CShapeBasedSeg* const shapeBasedSeg) : vnl_cost_function()
{
  m_shapeBasedSeg = shapeBasedSeg;
}


/*==================================================*/
void CShapeBasedSegCost::compute(vnl_vector< double > const &x, double *f, vnl_vector< double > *g)
{
  m_shapeBasedSeg->getCostAndGradient(x, f, g); // Yezzi eng

  //  m_shapeBasedSeg->getCostAndGradient1(x, f, g); // edge eng

  //  std::cout<<"mse = "<<*f<<std::endl;
}

