#ifndef __itkPolynomialSVMKernel_h
#define __itkPolynomialSVMKernel_h

#include "itkSVMKernelBase.h"
#include "itkNumericTraits.h"

#include "itkMeasurementVectorTraits.h"

namespace itk{ 
namespace Statistics{


template<class TVector, class TOutput>
class PolynomialSVMKernel : public SVMKernelBase<TVector, TOutput>
{
public:
  /** Standard "Self" typedef. */
  typedef PolynomialSVMKernel Self;
  typedef SVMKernelBase<TVector, TOutput> Superclass;
  typedef SmartPointer< Self > Pointer ; 
  typedef SmartPointer<const Self> ConstPointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(PolynomialSVMKernel, SVMKernelBase);
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self) ;
  
  /** Evaluates K(x1,x2) = (\gamma x1^t x2 + R)^{Power} */
  TOutput Evaluate(const TVector &x1, const TVector &x2) const ;
  
  itkSetMacro(Gamma,double);
  itkGetMacro(Gamma,double);
  
  itkSetMacro(R, double);
  itkGetMacro(R, double);

  itkSetMacro(Power, double);
  itkGetMacro(Power, double);

protected:
  PolynomialSVMKernel();           // not implemented
  virtual ~PolynomialSVMKernel();  // not implemented
  virtual void PrintSelf( std::ostream& os, Indent indent ) const;
  
private:
  double m_Gamma;
  double m_R;
  double m_Power;
  
} ; // end of class
  
} // end of namespace Statistics 
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPolynomialSVMKernel.txx"
#endif

#endif







