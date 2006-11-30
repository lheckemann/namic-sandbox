#ifndef __itkPolynomialSVMKernel_txx
#define __itkPolynomialSVMKernel_txx

#include "itkPolynomialSVMKernel.h"

#include <math.h>

namespace itk
{
namespace Statistics
{

/** Constructor */
template<class TVector, class TOutput>
PolynomialSVMKernel<TVector,TOutput>
::PolynomialSVMKernel()
  : m_Gamma(1.0), m_R(1.0), m_Power(2.0)
{}

/** Destructor */
template<class TVector, class TOutput>
PolynomialSVMKernel<TVector,TOutput>
::~PolynomialSVMKernel()
{}

/** Evaluate function */
template<class TVector, class TOutput>
TOutput PolynomialSVMKernel<TVector,TOutput>
::Evaluate(const TVector& x1,const TVector& x2 ) const
{ 
  assert(x1.Size() == x2.Size());

  double innerProduct = 0;
  for(unsigned int i = 0; i < x1.Size(); ++i)
  {
    innerProduct += x1[i] * x2[i];
  }

  return pow(m_Gamma * innerProduct + m_R, m_Power);
}

/** Print the object */
template<class TVector, class TOutput>
void  
PolynomialSVMKernel<TVector, TOutput>
::PrintSelf( std::ostream& os, Indent indent ) const 
{ 
  Superclass::PrintSelf( os, indent ); 
  os << indent << "PolynomialSVMKernel(" << this << ")" << std::endl; 
  os << indent << "Gamma = " << this->m_Gamma << std::endl;
  os << indent << "R = " << this->m_R << std::endl;
  os << indent << "D = " << this->m_Power << std::endl;
} 

} // end namespace Statistics
} // end namespace itk

#endif // __itkPolynomialSVMKernel_txx
