/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkKdTreeBasedKmeansEstimatorTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/08 03:18:41 $
  Version:   $Revision: 1.10 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <fstream>

#include "itkFixedArray.h"
#include "itkPoint.h"
#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"

#include "itkPointSetToListAdaptor.h"
#include "itkSubsample.h"
#include "itkListSample.h"
#include "itkKdTree.h"
#include "itkEuclideanDistance.h"
#include "itkWeightedCentroidKdTreeGenerator.h"
#include "itkKdTreeBasedKmeansEstimator.h"

int main(int argc, char* argv[] )
{
  namespace stat = itk::Statistics ;
 
  if (argc < 2)
    {
      std::cout << "ERROR: data file name argument missing." 
                << std::endl ;
      return EXIT_FAILURE;
    }

  unsigned int i, j ;
  const char* dataFileName = argv[1] ;

  int dataSize = 2000 ;
  int bucketSize = 10 ;

  double minStandardDeviation =28.54746 ;

  typedef itk::Array< double > ArrayType;

  itk::Array< double > trueMeans(4) ;
  trueMeans[0] = 99.261 ;
  trueMeans[1] = 100.078 ;
  trueMeans[2] = 200.1 ;
  trueMeans[3] = 201.3 ;

  itk::Array< double > initialMeans(4) ;
  initialMeans[0] = 80.0 ;
  initialMeans[1] = 80.0 ;
  initialMeans[2] = 180.0 ;
  initialMeans[3] = 180.0 ;
  int maximumIteration = 200 ;

  const unsigned int PointDimension = 3;

  double temp ;
  std::ifstream dataStream(dataFileName) ;
  while ( !dataStream.eof() )
    {
    for ( i = 0 ; i < PointDimension ; i++)
      {
      dataStream >> temp ;
      }
    }

  dataStream.close() ;
  
  /* Importing the point set to the sample */
  typedef itk::Statistics::ListSample< ArrayType >  DataSampleType;

  DataSampleType::Pointer sample = DataSampleType::New() ;
  
  sample->SetPointSet(pointSet);

  /* Creating k-d tree */
  typedef stat::WeightedCentroidKdTreeGenerator< DataSampleType > Generator ;
  Generator::Pointer generator = Generator::New() ;
  
  generator->SetSample(sample.GetPointer()) ;
  generator->SetBucketSize(bucketSize) ;
  generator->GenerateData() ;

  /* Searching kmeans */
  typedef stat::KdTreeBasedKmeansEstimator< Generator::KdTreeType > Estimator ;
  Estimator::Pointer estimator = Estimator::New() ;

  estimator->SetParameters(initialMeans) ;
  estimator->SetMaximumIteration(maximumIteration) ;
  estimator->SetKdTree(generator->GetOutput()) ;
  estimator->SetCentroidPositionChangesThreshold(0.0) ;
  estimator->StartOptimization() ;
  Estimator::ParametersType estimatedMeans = estimator->GetParameters() ;

  bool passed = true ;
  int index ;
  unsigned int numberOfMeasurements = DataSampleType::MeasurementVectorSize ;
  unsigned int numberOfClasses = trueMeans.size() / numberOfMeasurements ;
  for (i = 0 ; i < numberOfClasses ; i++)
    {
      std::cout << "cluster[" << i << "] " << std::endl ;
      double displacement = 0.0 ;
      std::cout << "    true mean :" << std::endl ;
      std::cout << "        " ;
      index = numberOfMeasurements * i ;
      for (j = 0 ; j < numberOfMeasurements ; j++)
        {
          std::cout << trueMeans[index] << " " ;
          ++index ;
        }
      std::cout << std::endl ;
      std::cout << "    estimated mean :" << std::endl ;
      std::cout << "        "  ;

      index = numberOfMeasurements * i ;
      for (j = 0 ; j < numberOfMeasurements ; j++)
        {
          std::cout << estimatedMeans[index] << " " ;
          temp = estimatedMeans[index] - trueMeans[index] ;
          ++index ;
          displacement += (temp * temp) ;
        }
      std::cout << std::endl ;
      displacement = sqrt(displacement) ;
      std::cout << "    Mean displacement: " << std::endl ;
      std::cout << "        " << displacement 
                << std::endl << std::endl ;
      // if the displacement of the estimates are within 3 % of
      // standardDeviation then we assume it is successful
      if ( displacement > (minStandardDeviation / 100.0) * 3 )
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







