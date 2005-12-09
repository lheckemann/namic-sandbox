
#ifndef __CommandIterationUpdate_h_
#define __CommandIterationUpdate_h_

#include "itkCommand.h"
#include <fstream>



namespace itk
{

class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef  itk::SmartPointer<Self>  Pointer;

  typedef  std::ofstream            StreamType;

  itkNewMacro( Self );
protected:
  CommandIterationUpdate() 
    {
    plotfile.open("plot.txt");  
    };
  ~CommandIterationUpdate()
    {
    plotfile << std::endl << std::endl;
    plotfile.close();
    }
public:
  typedef   itk::RegularStepGradientDescentBaseOptimizer  OptimizerType;
  typedef   const OptimizerType   *                   OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      OptimizerPointer optimizer = 
        dynamic_cast< OptimizerPointer >( object );
      if( typeid( event ) != typeid( itk::IterationEvent ) )
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

      const double gradientMagnitude = sqrt( magnitude2 );
 

      std::cout << optimizer->GetCurrentIteration() << "   ";
      std::cout << optimizer->GetValue() << "   ";
      std::cout << gradientMagnitude << "   ";
      std::cout << optimizer->GetCurrentStepLength() << "   ";
      std::cout << optimizer->GetCurrentPosition() << std::endl;

      plotfile << optimizer->GetCurrentIteration() << "   ";
      plotfile << optimizer->GetValue() << "   " << std::endl;
    }
private:
  StreamType plotfile;
};



} // end namespace itk


#endif

