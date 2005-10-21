/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBayesianClassifierImageFilterTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/03/09 20:58:34 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkGaussianDensityFunction.h"
#include "itkVectorContainer.h"
#include "itkBayesianClassifierInitializationImageFilter.h"
#include "itkBayesianClassifierImageFilter.h"
#include "itkImageFileWriter.h"

//int itkBayesianClassifierImageFilterTest(int argc, char* argv[] )
int main(int argc, char* argv[] )
{

  if( argc < 2 ) 
    { 
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImageFile outputImageFile" << std::endl;
    return EXIT_FAILURE;
    }

  // input parameters
  char * rawDataFileName = argv[1];
  char * labelMapFileName = argv[2];

  // setup reader
  const unsigned int Dimension = 2;
  typedef unsigned short InputPixelType;
  typedef itk::Image< InputPixelType, Dimension > InputImageType;
  typedef itk::ImageFileReader< InputImageType >     ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( rawDataFileName );

  // setup filters
  typedef unsigned short OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
  typedef itk::BayesianClassifierInitializationImageFilter<
                                 InputImageType,
                                 OutputImageType > InitializationFilterType;

  InitializationFilterType::Pointer filter = InitializationFilterType::New();
  filter->SetInput( reader->GetOutput() );

  filter->Update();

//   typedef itk::BayesianClassifierImageFilter< 
//                                  InputImageType,
//                                  OutputImageType > ClassifierFilterType;

//   ClassifierFilterType::Pointer filter = ClassifierFilterType::New();
//   filter->SetInput( reader->GetOutput() );


  // INITIALIZE FILTER
  // set number of classes here


  // SET FILTER'S PRIOR PARAMETERS
  // do nothing here to default to uniform priors
  // otherwise set the priors to some user provided values


  // CREATE GAUSSIAN MEMBERSHIP FUNCTIONS
//   typedef ClassifierFilterType::MeasurementVectorType     MeasurementVectorType;
//   typedef itk::Statistics::GaussianDensityFunction< MeasurementVectorType >
//                                                           MembershipFunctionType;
//   typedef MembershipFunctionType::Pointer                 MembershipFunctionPointer;
//   typedef itk::Array< double >                            ArrayType; // temp
//   std::vector< MembershipFunctionPointer >   membershipFunctions;
//   typedef itk::VectorContainer< unsigned long, MembershipFunctionType::MeanType* > 
//                                              MeanEstimatorsContainerType;
//   typedef itk::VectorContainer< unsigned long, MembershipFunctionType::CovarianceType* > 
//                                              CovarianceEstimatorsContainerType;
//   MeanEstimatorsContainerType::Pointer       meanEstimatorsContainer =
//                                                MeanEstimatorsContainerType::New();
//   CovarianceEstimatorsContainerType::Pointer covarianceEstimatorsContainer =
//                                                CovarianceEstimatorsContainerType::New();

//   meanEstimatorsContainer->Reserve( numberOfClasses );
//   covarianceEstimatorsContainer->Reserve( numberOfClasses );
//   ArrayType                                    estimatedMeans( numberOfClasses ); // temp
//   ArrayType                                    estimatedCovariances( numberOfClasses ); // temp

//   for ( unsigned int i = 0; i < numberOfClasses; ++i )
//     {
//     estimatedMeans[i] = 255*(i) / numberOfClasses; // temp
//     estimatedCovariances[i] = 100; // temp
    
//     meanEstimatorsContainer->InsertElement( i, new MembershipFunctionType::MeanType(1) );
//     covarianceEstimatorsContainer->
//                              InsertElement( i, new MembershipFunctionType::CovarianceType() );
//     MembershipFunctionType::MeanType*       meanEstimators = 
//                                       const_cast< MembershipFunctionType::MeanType * >
//                                       (meanEstimatorsContainer->GetElement(i));
//     MembershipFunctionType::CovarianceType* covarianceEstimators = 
//                                       const_cast< MembershipFunctionType::CovarianceType * >
//                                       (covarianceEstimatorsContainer->GetElement(i));
//     meanEstimators->SetSize(1);
//     covarianceEstimators->SetSize( 1, 1 );

//     meanEstimators->Fill( estimatedMeans[i] );
//     covarianceEstimators->Fill( estimatedCovariances[i] );
//     membershipFunctions.push_back( MembershipFunctionType::New() );
//     membershipFunctions[i]->SetMean( meanEstimatorsContainer->GetElement( i ) );
//     membershipFunctions[i]->SetCovariance( covarianceEstimatorsContainer->GetElement( i ) );
//     filter->AddMembershipFunction( membershipFunctions[i] );
//     }


  // SET FILTER'S SMOOTHING PARAMETERS
  // do nothing here to use default smoothing parameters
  // otherwise set the smoothing parameters


  // SETUP WRITER
//   typedef itk::ImageFileWriter< OutputImageType >    WriterType;

//   WriterType::Pointer writer = WriterType::New();
//   writer->SetFileName( labelMapFileName );


//   // WRITE LABELMAP TO FILE
//   // need to write GetOutput method
//   writer->SetInput( filter->GetOutput() );
//   writer->SetFileName( labelMapFileName );
//   try
//     {
//     writer->Update();
//     }
//   catch( itk::ExceptionObject & excp )
//     {
//     std::cerr << "Problem encoutered while writing image file : "
//       << argv[2] << std::endl;
//     std::cerr << excp << std::endl;
//     return EXIT_FAILURE;
//     }

  
//   for ( unsigned int i = 0; i < numberOfClasses; ++i )
//     {
//     MembershipFunctionType::MeanType*       meanEstimators =
//       const_cast< MembershipFunctionType::MeanType * >
//       (meanEstimatorsContainer->GetElement(i));
//     MembershipFunctionType::CovarianceType* covarianceEstimators =
//       const_cast< MembershipFunctionType::CovarianceType * >
//       (covarianceEstimatorsContainer->GetElement(i));
//     delete meanEstimators;
//     delete covarianceEstimators;
//     meanEstimatorsContainer->DeleteIndex(i);
//     covarianceEstimatorsContainer->DeleteIndex(i);
//     }
 
  // TESTING PRINT
  filter->Print( std::cout );
  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}
