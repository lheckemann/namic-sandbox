#include <iostream>
#include <cstdlib>

#include "itkFRPROptimizer.h"
#include "itkSingleValuedCostFunction.h"
#include "itkArray.h"
#include "itkVariableSizeMatrix.h"
#include "itkSmartPointer.h"

class Paraboloid : public itk::SingleValuedCostFunction
{
public:
  typedef Paraboloid Self;
  typedef itk::SingleValuedCostFunction Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::SmartPointer<const Self>     ConstPointer;

  itkNewMacro(Self);

  itkTypeMacro(Paraboloid, itk::SingleValuedCostFunction);

  typedef Superclass::ParametersType Parameters;
  typedef Superclass::DerivativeType Derivative;
  
  const static unsigned int Dimensions = 12;

  itkSetMacro( Center, itk::Array<double>)
  itkSetMacro( Ellipsoid, itk::VariableSizeMatrix<double>)

  virtual unsigned int GetNumberOfParameters() const
  {
    return Dimensions;
  }

  virtual double GetValue(const Parameters & p) const
  {
    return (p - m_Center).two_norm();
  }

  virtual void GetDerivative(const Parameters & p, DerivativeType & d) const
  {
    d = (p - m_Center);
    d = 2.0 * d;
  }
  
protected:
  Paraboloid() : m_Center(Dimensions), m_Ellipsoid(Dimensions, Dimensions)
  {
    m_Center.Fill(0.0);
    m_Ellipsoid.SetIdentity();
  }
  
  virtual ~Paraboloid() {}

private:
  Paraboloid(const Self&);
  void operator=(const Self&);
  
  itk::Array<double> m_Center;
  itk::VariableSizeMatrix<double> m_Ellipsoid;
};

int FRPROptimizerTest(int argc, char* argv[])
{
  typedef itk::FRPROptimizer Optimizer;
  Optimizer::Pointer opt = Optimizer::New();

  Paraboloid::Pointer p = Paraboloid::New();
  itk::Array<double> c(p->GetNumberOfParameters());
  
  for(unsigned int i = 0; i < c.GetNumberOfElements(); ++i)
    {
    c[i] = i;
    }
        
  p->SetCenter(c);
  
  std::cout << p->GetValue(c) << std::endl;

  return EXIT_SUCCESS;
}
