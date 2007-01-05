
#ifndef __CommandStopUpdate_h_
#define __CommandStopUpdate_h_

#include "itkCommand.h"
#include "itkRegularStepGradientDescentBaseOptimizer.h"



namespace itk
{

class CommandStopUpdate : public itk::Command 
{
public:
  typedef  CommandStopUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef  itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandStopUpdate() {};
public:
  typedef   itk::RegularStepGradientDescentBaseOptimizer  OptimizerType;
  typedef   const OptimizerType*                      OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {

      OptimizerPointer optimizer = dynamic_cast< OptimizerPointer >( object );

      if( typeid( event ) != typeid( itk::EndEvent ) )
        {
        return;
        }

      OptimizerType::DerivativeType gradient = optimizer->GetGradient();
      OptimizerType::ScalesType     scales   = optimizer->GetScales();

      double magnitude2 = 0.0;

      for(unsigned int i=0; i<gradient.size(); i++)
        {
        const double fc = gradient[i] / scales[i];
        magnitude2 += fc * fc;
        }  

      OptimizerType::StopConditionType stopCondition = optimizer->GetStopCondition();
      const double magnitude = sqrt( magnitude2 );
      
      std::cout << "Optimizer stopped : " << std::endl;
      std::cout << "Stop condition   =  " << stopCondition << std::endl;
      switch(stopCondition)
      {
        case OptimizerType::GradientMagnitudeTolerance:
          std::cout << "GradientMagnitudeTolerance" << std::endl; 
          break;
        case OptimizerType::StepTooSmall:
          std::cout << "StepTooSmall" << std::endl; 
          break;
        case OptimizerType::ImageNotAvailable:
          std::cout << "ImageNotAvailable" << std::endl; 
          break;
        case OptimizerType::MaximumNumberOfIterations:
          std::cout << "MaximumNumberOfIterations" << std::endl; 
          break;
      }
      std::cout << "Current value    =  " << optimizer->GetValue() << std::endl;
      std::cout << "Current Position =  " << optimizer->GetCurrentPosition() << std::endl;
      std::cout << "Current Step Len =  " << optimizer->GetCurrentStepLength() << std::endl;
      std::cout << "Current Iteration=  " << optimizer->GetCurrentIteration() << std::endl;
      std::cout << "Current Gradient =  " << gradient    << std::endl;
      std::cout << "Gradient Tol.    =  " << optimizer->GetGradientMagnitudeTolerance() << std::endl;
      std::cout << "Gradient Mag.    =  " << magnitude   << std::endl;
    }
};



} // end namespace itk


#endif

