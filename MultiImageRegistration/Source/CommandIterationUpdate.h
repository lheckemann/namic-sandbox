
/*
 *  CommandIterationupdate.h
 *  ThirionCLP
 *
 *  Created by Yong Qiang Zhao on 01/27/10.
 *  Copyright 2008 __UI__. All rights reserved.
 *
 */
#include "itkCommand.h"
#include "itkImage.h"
#include "MultiImageMetric.h"
#include <string>

//Bspline optimizer and transform
#include "itkGradientDescentOptimizer.h"
#include "GradientDescentLineSearchOptimizer.h"
#include "itkSPSAOptimizer.h"
#include "itkAmoebaOptimizer.h"

class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef  itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
  typedef itk::Image< float, 3 > InternalImageType;
protected:
  CommandIterationUpdate(): m_CumulativeIterationIndex(0)
  {
    m_MetricPrint = false;
    m_PrintInterval = 500;
  };
public:
  
  typedef   itk::SingleValuedNonLinearOptimizer   OptimizerType;
  typedef   const OptimizerType *                            OptimizerPointer;
  typedef   itk::GradientDescentOptimizer       GradientOptimizerType;
  typedef   const GradientOptimizerType *             GradientOptimizerPointer;
  typedef   itk::GradientDescentLineSearchOptimizer LineSearchOptimizerType;
  typedef   const LineSearchOptimizerType  *          LineSearchOptimizerPointer;
  typedef   itk::SPSAOptimizer                        SPSAOptimizerType;
  typedef   const SPSAOptimizerType  *                SPSAOptimizerPointer;
  typedef   itk::AmoebaOptimizer                        SimplexOptimizerType;
  typedef   const SimplexOptimizerType  *                SimplexOptimizerTypePointer;
  typedef   itk::MultiImageMetric< InternalImageType >    MetricType;


  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      OptimizerPointer optimizer = dynamic_cast< OptimizerPointer >( object );
      if( !(itk::IterationEvent().CheckEvent( &event )) )
      {
        return;
      }

      // only print results every ten iterations
      if(m_CumulativeIterationIndex % 1 != 0 )
      {
        m_CumulativeIterationIndex++;
        return;
      }
      
      if(  !strcmp(optimizer->GetNameOfClass(), "GradientDescentOptimizer" ) )
      {
        GradientOptimizerPointer gradientPointer =
            dynamic_cast< GradientOptimizerPointer >(object );
        std::cout << std::setiosflags(std::ios::fixed) << std::showpoint << std::setfill('0');
        std::cout << "Iter " << std::setw(3) << m_CumulativeIterationIndex << "   ";
        std::cout << std::setw(3) << gradientPointer->GetCurrentIteration() << "   ";
        std::cout << std::setw(6) << gradientPointer->GetValue() << "   " << std::endl;
        if(gradientPointer->GetCurrentIteration() % 50 == 0)
        {
          //std::cout << std::setw(6) << "Position: " << gradientPointer->GetCurrentPosition() << std::endl;
        }
      }

      else if(!strcmp(optimizer->GetNameOfClass(), "GradientDescentLineSearchOptimizer") )
      {
        LineSearchOptimizerPointer lineSearchOptimizerPointer =
            dynamic_cast< LineSearchOptimizerPointer >( object );
        std::cout << std::setiosflags(std::ios::fixed) << std::showpoint << std::setfill('0');
        std::cout << "Iter "<< std::setw(3) << m_CumulativeIterationIndex << "   ";
        std::cout << std::setw(3) << lineSearchOptimizerPointer->GetCurrentIteration() << "   ";
        std::cout << std::setw(6) << lineSearchOptimizerPointer->GetValue() << "   " << std::endl;
        if(m_CumulativeIterationIndex % 50 == 0 )
        { 
          //std::cout << std::setw(6) << "Position: " << lineSearchOptimizerPointer->GetCurrentPosition() << std::endl;
        }
      }

      else if(!strcmp(optimizer->GetNameOfClass(), "SPSAOptimizer") )
      {
        SPSAOptimizerPointer spsaOptimizerPointer =
            dynamic_cast< SPSAOptimizerPointer >( object );
        std::cout << std::setiosflags(std::ios::fixed) << std::showpoint << std::setfill('0');
        std::cout << "Iter "<< std::setw(3) << m_CumulativeIterationIndex << "   ";
        std::cout << std::setw(3) << spsaOptimizerPointer->GetCurrentIteration() << "   ";
        std::cout << std::setw(6) << spsaOptimizerPointer->GetValue() << "   " << std::endl;
        if(m_CumulativeIterationIndex % 100 == 0 )
        {
          typedef SPSAOptimizerType::ParametersType ParametersType;
          ParametersType parameters = spsaOptimizerPointer->GetCurrentPosition();
          double max = -1e308;
          for(unsigned int i=0; i< parameters.Size(); i++)
          {
            if(parameters[i]> max)
            {
               max = parameters[i];
            }
          }
          std::cout << std::setw(6) << "Max Parameter: " << max << std::endl;
        }
      }

      else if(!strcmp(optimizer->GetNameOfClass(), "AmoebaOptimizer") )
      {
        SimplexOptimizerTypePointer simplexOptimizer =
            dynamic_cast< SimplexOptimizerTypePointer >( object );
        std::cout << std::setiosflags(std::ios::fixed) << std::showpoint << std::setfill('0');
        std::cout << "Iter "<< std::setw(3) << m_CumulativeIterationIndex << "   ";
        std::cout << std::setw(6) << simplexOptimizer->GetCachedValue() << "   " << std::endl;
        if(m_CumulativeIterationIndex % 100 == 0 )
        {
          typedef SimplexOptimizerType::ParametersType ParametersType;
          ParametersType parameters = simplexOptimizer->GetCurrentPosition();
          double max = -1e308;
          for(unsigned int i=0; i< parameters.Size(); i++)
          {
            if(parameters[i]> max)
            {
              max = parameters[i];
            }
          }
          std::cout << std::setw(6) << "Max Parameter: " << max << std::endl;
        }
      }

      // only print results every ten iterations
      if(m_MetricPrint && m_CumulativeIterationIndex % m_PrintInterval == 0 )
      {
         // Print the metric Value
        unsigned long int numberOfSamples = m_MetricPointer->GetNumberOfSpatialSamples();
        m_MetricPointer->Finalize();
        m_MetricPointer->SetNumberOfSpatialSamples( 1000000 );
        m_MetricPointer->Initialize();
        std::cout << "ALLSamples: Iter" << m_CumulativeIterationIndex;
        std::cout << " Value " << m_MetricPointer->GetValue(optimizer->GetCurrentPosition());
        std::cout << " # of samples " << m_MetricPointer->GetFixedImageRegion().GetNumberOfPixels()/2 << std::endl;
        m_MetricPointer->Finalize();
        m_MetricPointer->SetNumberOfSpatialSamples(numberOfSamples);
        m_MetricPointer->Initialize();

      }
      
      //Increase the cumulative index
      m_CumulativeIterationIndex++;
      //std::cout << std::setw(6) << optimizer->GetCurrentPosition()[i] << "   ";

      
    }

    MetricType::Pointer m_MetricPointer;
    bool m_MetricPrint;
    unsigned int m_PrintInterval;
private:
    unsigned int m_CumulativeIterationIndex;
  
};

