/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkGoodnessOfFitMixtureModelCostFunctionTest.cxx,v $
Language:  C++
Date:      $Date: 2009-04-06 19:25:07 $
Version:   $Revision: 1.15 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include "itkWin32Header.h"

#include <fstream>

#include "itkPoint.h"
#include "itkPointSet.h"
#include "itkArray.h"

#include "itkPointSetToListAdaptor.h"
#include "itkHistogram.h"
#include "itkSubsample.h"
#include "itkListSampleToHistogramFilter.h"
#include "itkGaussianGoodnessOfFitComponent.h"
#include "itkLogLikelihoodGoodnessOfFitFunction.h"
#include "itkNormalVariateGenerator.h"
#include "itkOnePlusOneEvolutionaryOptimizer.h"
#include "itkGoodnessOfFitMixtureModelCostFunction.h"

#define NO_OF_DIMENSIONS 2
typedef double ValueType ;

int itkGoodnessOfFitMixtureModelCostFunctionTest(int argc, char* argv[] )
{
  namespace stat = itk::Statistics ;
 
  if (argc < 2)
    {
      std::cout << "ERROR: data file name argument missing." 
                << std::endl ;
      return EXIT_FAILURE;
    }

  unsigned int i, j ;
  char* dataFileName = argv[1] ;
  double minStandardDeviation =28.54746 ;

  std::vector< double > trueParams(6) ;
  trueParams[0] = 99.261 ;
  trueParams[1] = 100.078 ;
  trueParams[2] = 28.59448 ;
  trueParams[3] = 200.1 ;
  trueParams[4] = 201.3 ;
  trueParams[5] = 29.32210 ;
    
  std::vector< double > initialParams(6) ;
  initialParams[0] = 90.0 ;
  initialParams[1] = 90.0 ;
  initialParams[2] = 28.59448 ;
  //initialParams[2] = 15 ;
  initialParams[3] = 190.0 ;
  initialParams[4] = 190.0;
  initialParams[5] = 29.32210 ;
  //initialParams[5] = 15 ;

  std::vector< double > standardDeviations ;
  int maxIteration = 2000 ;
  int dataSize = 2000 ;
  double histogramOverlap = 0.75 ;
  double histogramExtent = 1.5 ;
  unsigned int numberOfClasses = 2 ;

  /* Loading point data */
  typedef itk::PointSet< double, 2 > PointSetType ;
  PointSetType::Pointer pointSet = PointSetType::New() ;
  PointSetType::PointsContainerPointer pointsContainer = 
    PointSetType::PointsContainer::New() ;
  pointsContainer->Reserve(dataSize) ;
  pointSet->SetPoints(pointsContainer.GetPointer()) ;

  PointSetType::PointsContainerIterator p_iter = pointsContainer->Begin() ;
  PointSetType::PointType point ;
  double temp ;
  std::ifstream dataStream(dataFileName) ;
  if ( !dataStream )
    {
      std::cout << "ERROR: fail to open the data file." << std::endl ;
      return EXIT_FAILURE ;
    }

  while (p_iter != pointsContainer->End())
    {
      for ( i = 0 ; i < PointSetType::PointDimension ; i++)
        {
          dataStream >> temp ;
          point[i] = temp ;
        }
      p_iter.Value() = point ;
      ++p_iter ;
    }

  dataStream.close() ;

  /* Importing the point set to the sample */
  typedef stat::PointSetToListAdaptor< PointSetType >
    DataSampleType;

  DataSampleType::Pointer sample =
    DataSampleType::New() ;
  
  sample->SetPointSet(pointSet);

  /* =============================================== */
  typedef stat::GoodnessOfFitMixtureModelCostFunction< DataSampleType > 
    CostFunctionType ;
  
  CostFunctionType::Pointer costFunction = CostFunctionType::New() ;

  //  costFunction->SetInputSample(histogram.GetPointer()) ;

  typedef stat::GaussianGoodnessOfFitComponent< DataSampleType >
    ComponentType ;

  std::vector< ComponentType::Pointer > components ;

  for ( i = 0 ; i < numberOfClasses ; i++)
    {
      components.push_back(ComponentType::New()) ;
      components[i]->SetHistogramNumberOfBins(6) ;
      components[i]->SetHistogramExtent(histogramExtent) ;
      components[i]->SetHistogramBinOverlap(histogramOverlap) ;
      components[i]->SetInputSample(sample.GetPointer()) ;

      costFunction->AddComponent(components[i].GetPointer()) ;
    }

  /* Initializing the goodness of fit function */
  typedef ComponentType::HistogramType ObservedHistogramType ;

  typedef stat::LogLikelihoodGoodnessOfFitFunction< ComponentType::HistogramType > FunctionType ;
  
  FunctionType::Pointer function = FunctionType::New() ;
  
  costFunction->SetFunction(function.GetPointer()) ;

  typedef stat::NormalVariateGenerator 
    RandomVariateGeneratorType ;
  RandomVariateGeneratorType::Pointer randomGenerator = 
    RandomVariateGeneratorType::New() ;
  randomGenerator->Initialize(3024) ;

  typedef itk::OnePlusOneEvolutionaryOptimizer
    OptimizerType ;
  
  unsigned int paramSize = costFunction->GetNumberOfParameters() ;
  CostFunctionType::ParametersType params(paramSize) ;

  for ( i = 0 ; i < paramSize ; i++ )
    {
      params[i] = initialParams[i] ;
    }

  double initStepSize = 1.05 ;
  double grow = 1.1 ;
  double shrink = vcl_pow(grow, -0.25) ;
  OptimizerType::ScalesType scales(paramSize) ;
  scales.Fill(1.0) ;
  for ( i = 0 ; i < paramSize ; i++)
    {
      if ( i % (NO_OF_DIMENSIONS + 1) == 2 )
        {
          scales[i] = 10000.0 ;
        }
    }

  OptimizerType::Pointer optimizer = OptimizerType::New() ;
  optimizer->SetCostFunction(costFunction.GetPointer()) ;
  optimizer->Initialize(initStepSize, grow, shrink) ;
  optimizer->SetMaximumIteration(maxIteration) ;
  optimizer->SetNormalVariateGenerator(randomGenerator.GetPointer()) ;
  optimizer->SetInitialPosition(params) ;
  optimizer->SetScales(scales) ;
  optimizer->SetEpsilon(1.0e-6) ;
  //  optimizer->DebugOn() ;

  /* Optimizing */
  optimizer->StartOptimization() ;

  std::cout << "DEBUG: current iteration = " 
            << optimizer->GetCurrentIteration() << std::endl ;

  bool passed = true ;
  double displacement ;
  unsigned int paramIndex = 0 ;
  for ( i = 0 ; i < numberOfClasses ; i++)
    {
      std::cout << "Cluster[" << i << "]" << std::endl ;
      std::cout << "    Parameters:" << std::endl ;
      std::cout << "         " << (components[i])->GetFullParameters() << std::endl ;
      displacement = 0.0 ;
      for ( j = 0 ; j < DataSampleType::MeasurementVectorSize ;
           j++)
        {
          temp = (components[i])->GetFullParameters()[j] - trueParams[paramIndex] ;
          displacement += (temp * temp) ;
          paramIndex++ ;
        }
      paramIndex++ ;
      displacement = vcl_sqrt(displacement) ;
      std::cout << "    Mean displacement: " << std::endl ;
      std::cout << "        " << displacement 
                << std::endl << std::endl ;
      if ( displacement > (minStandardDeviation / 100.0) * 10 )
        {
          passed = false ;
        }
    }

  if( !passed )
    {
      std::cout << "Test failed." << std::endl;
      return EXIT_FAILURE;
    }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}
