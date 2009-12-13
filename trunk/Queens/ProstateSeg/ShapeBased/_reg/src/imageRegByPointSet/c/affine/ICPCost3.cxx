#include "ICP3.h"
#include "ICPCost3.h"


//CICPCost3::CICPCost3( ICP3 *icp):vnl_cost_function( ICP3::m_numOfParameters )
CICPCost3::CICPCost3( ICP3 *icp):vnl_cost_function( )
{
  m_ICP3 = icp;
}


/*==================================================*/
void CICPCost3::compute(vnl_vector< double > const &x, double *f, vnl_vector< double > *g)
{
  m_ICP3->getCostAndGradient(x, f, g);
  //  std::cout<<"mse = "<<*f<<std::endl;
}

