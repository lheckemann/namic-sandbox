#include "itkCommand.h"
#include "itkImageRegistrationMethod.h"
#include "itkLBFGSBOptimizer.h"


// Use an anonymous namespace to keep class types and function names
// from colliding when module is used as shared object module.  Every
// thing should be in an anonymous namespace except for the module
// entry point, e.g. main()
//

namespace {

class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
  typedef itk::SingleValuedCostFunction CostFunctionType;
  void SetRegistration (itk::ProcessObject * obj)
    {
    m_Registration = obj; 
    }
  void SetCostFunction(CostFunctionType* fn)
    {
      m_CostFunction = fn;
    }
protected:
  CommandIterationUpdate() {};
  itk::ProcessObject::Pointer m_Registration;
  CostFunctionType::Pointer m_CostFunction;
public:
  typedef itk::LBFGSBOptimizer  OptimizerType;
  typedef OptimizerType   *OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      OptimizerPointer optimizer = 
        dynamic_cast< OptimizerPointer >( const_cast<itk::Object *>(object) );
      if( !(itk::IterationEvent().CheckEvent( &event )) )
        {
        return;
        }
      
      if (m_Registration)
        {
        m_Registration->UpdateProgress( 
          static_cast<double>(optimizer->GetCurrentIteration()) /
          static_cast<double>(optimizer->GetMaximumNumberOfIterations()));
        }
    }
};

}


