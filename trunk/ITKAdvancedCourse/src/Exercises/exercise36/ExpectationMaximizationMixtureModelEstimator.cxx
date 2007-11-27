/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ExpectationMaximizationMixtureModelEstimator.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/19 16:31:51 $
  Version:   $Revision: 1.13 $

     Copyright (c) Insight Software Consortium. All rights reserved.
     See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include "itkVector.h"
#include "itkImage.h"

#include "itkImageToListAdaptor.h"

#include "itkGaussianMixtureModelComponent.h"
#include "itkExpectationMaximizationMixtureModelEstimator.h"

#include "itkImageFileReader.h"


int main(int argc, char * argv [] )
{
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


  unsigned int numberOfClasses = 2;

  typedef itk::Array< double > ParametersType;
  ParametersType params( NumberOfComponents );

  std::vector< ParametersType > initialParameters( numberOfClasses );
  params[0] = 110.0;
  params[1] = 800.0;
  params[2] = 500.0;
  initialParameters[0] = params;

  params[0] = 210.0;
  params[1] = 850.0;
  params[2] = 450.0;
  initialParameters[1] = params;

  typedef itk::Statistics::GaussianMixtureModelComponent< AdaptorType > 
    ComponentType;

  std::vector< ComponentType::Pointer > components;
  for ( unsigned int i = 0 ; i < numberOfClasses ; i++ )
    {
    components.push_back( ComponentType::New() );
    (components[i])->SetSample( adaptor );
    (components[i])->SetParameters( initialParameters[i] );
    }
  

  typedef itk::Statistics::ExpectationMaximizationMixtureModelEstimator< 
                           AdaptorType > EstimatorType;
  EstimatorType::Pointer estimator = EstimatorType::New();

  estimator->SetSample( adaptor );
  estimator->SetMaximumIteration( 200 );

  itk::Array< double > initialProportions(numberOfClasses);
  initialProportions[0] = 0.5;
  initialProportions[1] = 0.5;

  estimator->SetInitialProportions( initialProportions );
  
  for ( unsigned int i = 0 ; i < numberOfClasses ; i++)
    {
    estimator->AddComponent( (ComponentType::Superclass*)
                             (components[i]).GetPointer() );
    }
  
  estimator->Update();


  for ( unsigned int i = 0 ; i < numberOfClasses ; i++ )
    {
    std::cout << "Cluster[" << i << "]" << std::endl;
    std::cout << "    Parameters:" << std::endl;
    std::cout << "         " << (components[i])->GetFullParameters() 
              << std::endl;
    std::cout << "    Proportion: ";
    std::cout << "         " << (*estimator->GetProportions())[i] << std::endl;
    }
  
  return 0;
}







