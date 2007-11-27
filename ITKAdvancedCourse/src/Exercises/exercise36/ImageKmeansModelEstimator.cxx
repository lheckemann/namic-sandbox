/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ScalarImageKmeansModelEstimator.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/20 13:27:55 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include "itkKdTree.h"
#include "itkKdTreeBasedKmeansEstimator.h"
#include "itkWeightedCentroidKdTreeGenerator.h"

#include "itkImageToListAdaptor.h"

#include "itkImage.h"
#include "itkImageFileReader.h"

int main( int argc, char * argv [] )
{

  if( argc < 3 )
    {
    std::cerr << "Missing command line arguments" << std::endl;
    std::cerr << "Usage :  " << argv[0];
    std::cerr << "  inputImageFileName  numberOfClasses" << std::endl;
    return EXIT_FAILURE;
    }

  typedef unsigned char                     PixelValueType;
  const unsigned int                        NumberOfComponents = 3;

  typedef itk::Vector< PixelValueType, NumberOfComponents >  PixelType;

  const unsigned int                        Dimension = 2;

  typedef itk::Image<PixelType, Dimension > ImageType;

  typedef itk::ImageFileReader< ImageType > ReaderType;

  ReaderType::Pointer reader = ReaderType::New();

  reader->SetFileName( argv[1] );

  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  
  // Create a List from the scalar image
  typedef itk::Statistics::ImageToListAdaptor< ImageType >   AdaptorType;

  AdaptorType::Pointer adaptor = AdaptorType::New();

  adaptor->SetImage(  reader->GetOutput() );



  // Define the Measurement vector type from the AdaptorType
  typedef AdaptorType::MeasurementVectorType  MeasurementVectorType;


  // Create the K-d tree structure
  typedef itk::Statistics::WeightedCentroidKdTreeGenerator< 
                                                      AdaptorType > 
                                                              TreeGeneratorType;

  TreeGeneratorType::Pointer treeGenerator = TreeGeneratorType::New();

  treeGenerator->SetSample( adaptor );
  treeGenerator->SetBucketSize( 16 );
  treeGenerator->Update();



  typedef TreeGeneratorType::KdTreeType TreeType;
  typedef itk::Statistics::KdTreeBasedKmeansEstimator<TreeType> EstimatorType;

  EstimatorType::Pointer estimator = EstimatorType::New();

  unsigned int numberOfClasses = atoi( argv[2] );

  std::cout << "Number of Classes = " << numberOfClasses << std::endl;

  EstimatorType::ParametersType initialMeans( numberOfClasses * NumberOfComponents );
  initialMeans.Fill( 0.0 );

  estimator->SetParameters( initialMeans );
  
  estimator->SetKdTree( treeGenerator->GetOutput() );
  estimator->SetMaximumIteration( 200 );
  estimator->SetCentroidPositionChangesThreshold(0.0);
  estimator->StartOptimization();

  EstimatorType::ParametersType estimatedMeans = estimator->GetParameters();

  for ( unsigned int i = 0 ; i < numberOfClasses ; ++i )
    {
    std::cout << "cluster[" << i << "] ";
    std::cout << "  estimated mean : ";
    for ( unsigned int j = 0 ; j < NumberOfComponents ; ++j )
      {
      std::cout << "    " << estimatedMeans[i*NumberOfComponents +j];
      }
    std::cout << std::endl;
    }

  return EXIT_SUCCESS;
  
}


