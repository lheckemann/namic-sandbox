/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageToListSampleAdaptorTest.cxx,v $
  Language:  C++
  Date:      $Date: 2007/01/15 18:38:35 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// The example tests the class itk::Statistics::ImageToListSampleAdaptor.

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImageToListSampleAdaptor.h"
#include "itkImageClassifierFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkFixedArray.h"
#include "itkGaussianMixtureModelComponent.h"
#include "itkExpectationMaximizationMixtureModelEstimator.h"
#include "itkMaximumDecisionRule2.h"
#include "itkImageToListSampleAdaptor.h"
#include "itkNormalVariateGenerator.h"

int itkImageClassifierFilterTest(int, char* [] ) 
{
  const unsigned int MeasurementVectorSize = 1;
  typedef double MeasurementComponentType;
  const unsigned int numberOfClasses = 2;
  typedef itk::FixedArray< MeasurementComponentType, MeasurementVectorSize > InputPixelType;

  const unsigned int ImageDimension = 2;
  typedef itk::Image< InputPixelType, ImageDimension > InputImageType;

  typedef unsigned int OutputPixelType;
  typedef itk::Image< OutputPixelType, ImageDimension > OutputImageType;
  
  //Generate an image with pixel intensities generated from two normal
  //distributions
  typedef itk::Statistics::NormalVariateGenerator NormalGeneratorType;
  NormalGeneratorType::Pointer normalGenerator = NormalGeneratorType::New();
  normalGenerator->Initialize( 101 );

  InputImageType::Pointer image = InputImageType::New();

  InputImageType::IndexType start;
  InputImageType::SizeType  size;

  start.Fill( 0 );
  size.Fill( 10 );

  InputImageType::RegionType region( start, size );
  image->SetRegions( region );
  image->Allocate();
  typedef itk::ImageRegionIteratorWithIndex< InputImageType > IteratorType;
  IteratorType it( image, region );
  it.GoToBegin();

  //Pixel intensities generated from the first normal distribution
  double mean = 10.5; 
  double standardDeviation = 0.1; 

  while (!it.IsAtEnd())
    {
    InputPixelType value;
    value[0] = (normalGenerator->GetVariate() * standardDeviation ) + mean;
    it.Set( value );
    ++it; 
    }

  //Pixel intensities generated from the second normal distribution
  double mean2 = 200.5; 
  double standardDeviation2 = 0.1; 

  while (!it.IsAtEnd())
    {
    InputPixelType value;
    value[0] = (normalGenerator->GetVariate() * standardDeviation2 ) + mean2;
    it.Set( value );
    ++it; 
    }

  //Instantiate an image to list sample adaptor to pass the sample list
  //to EM estimator
  typedef  itk::Statistics::ImageToListSampleAdaptor< InputImageType >
    ImageToListSampleAdaptorType ;

  ImageToListSampleAdaptorType::Pointer sample = ImageToListSampleAdaptorType::New() ;

  sample->SetImage( image );

  //Use EM estimator to estimate gaussian membership functions
  typedef itk::Statistics::ListSample< InputPixelType > SampleType;
  typedef itk::Statistics::ExpectationMaximizationMixtureModelEstimator< SampleType >
    EstimatorType ;
  typedef itk::Statistics::GaussianMixtureModelComponent< SampleType > 
    ComponentType ;

  /* Preparing the gaussian mixture components */
  typedef itk::Array < double > ParametersType;
  std::vector< ParametersType > initialParameters(numberOfClasses);
  ParametersType params(2);
  params[0] = 5;
  params[1] = 0.1;
  initialParameters[0] = params;

  params[0] = 150;
  params[1] = 0.1;
  initialParameters[0] = params;

  typedef ComponentType::Pointer ComponentPointer ;
  std::vector< ComponentPointer > components ;
  for (unsigned int i = 0 ; i < numberOfClasses ; i++ )
    {
      components.push_back(ComponentType::New()) ;
      (components[i])->SetSample(sample.GetPointer()) ;
      (components[i])->SetParameters(initialParameters[i]) ;
    }
  
  /* Estimating */
  EstimatorType::Pointer estimator = EstimatorType::New() ;
  estimator->SetSample(sample.GetPointer()) ;

  int maximumIteration = 200;
  estimator->SetMaximumIteration(maximumIteration) ;

  itk::Array< double > initialProportions(numberOfClasses) ;
  initialProportions[0] = 0.5 ;
  initialProportions[1] = 0.5 ;

  estimator->SetInitialProportions(initialProportions) ;

  for (unsigned int i = 0 ; i < numberOfClasses ; i++)
    {
      estimator->AddComponent((ComponentType::Superclass*) 
                              (components[i]).GetPointer()) ;
    }

  estimator->Update() ;



  typedef itk::Statistics::ImageClassifierFilter< SampleType, 
  InputImageType,OutputImageType > ImageClassifierFilterType;
  ImageClassifierFilterType::Pointer filter 
                              = ImageClassifierFilterType::New();

  typedef ImageClassifierFilterType::ClassLabelVectorObjectType               ClassLabelVectorObjectType;
  typedef ImageClassifierFilterType::ClassLabelVectorType                     ClassLabelVectorType;

  ClassLabelVectorObjectType::Pointer  classLabelsObject = ClassLabelVectorObjectType::New();

  // Add class labels
  ClassLabelVectorType & classLabelVector  = classLabelsObject->Get();

  typedef ImageClassifierFilterType::ClassLabelType        ClassLabelType;

  ClassLabelType  class1 = 0;
  classLabelVector.push_back( class1 );

  ClassLabelType  class2 = 1;
  classLabelVector.push_back( class2 );

  //Set a decision rule type
  typedef itk::Statistics::MaximumDecisionRule2  DecisionRuleType;

  DecisionRuleType::Pointer    decisionRule = DecisionRuleType::New();

  const ImageClassifierFilterType::MembershipFunctionVectorObjectType *
                membershipFunctionsObject = estimator->GetOutput();
 
  /* Print out estimated parameters of the membership function */

  const ImageClassifierFilterType::MembershipFunctionVectorType  
            membershipFunctions = membershipFunctionsObject->Get();

  ImageClassifierFilterType::MembershipFunctionVectorType::const_iterator
                    begin = membershipFunctions.begin();

  ImageClassifierFilterType::MembershipFunctionVectorType::const_iterator
                    end = membershipFunctions.end();

  ImageClassifierFilterType::MembershipFunctionVectorType::const_iterator functionIter;

  functionIter=begin;
  
  unsigned int counter=1;
  std::cout << "Estimator membership function output " << std::endl;
  while( functionIter != end )
    {
    ImageClassifierFilterType::MembershipFunctionPointer membershipFunction = *functionIter;
    const EstimatorType::GaussianMembershipFunction * 
          gaussianMemberShpFunction = 
        dynamic_cast<const EstimatorType::GaussianMembershipFunction*>(membershipFunction.GetPointer());
    std::cout << "\tMembership function:\t " << counter << std::endl;
    std::cout << "\t\tMean="<< gaussianMemberShpFunction->GetMean() << std::endl;
    std::cout << "\t\tCovariance matrix=" << gaussianMemberShpFunction->GetCovariance() << std::endl;
    functionIter++;
    counter++;
    }

  //Set membership functions weight array
  const ImageClassifierFilterType::MembershipFunctionsWeightsArrayObjectType * 
            weightArrayObjects = estimator->GetMembershipFunctionsWeightsArray(); 
  const ImageClassifierFilterType::MembershipFunctionsWeightsArrayType  weightsArray = weightArrayObjects->Get();

  std::cout << "Estimator membership function Weight/proporation output: " << std::endl;
  for(unsigned int i=0; i < weightsArray.Size(); i++ )
    {
    std::cout << "Membership function: \t" << i << "\t" << weightsArray[i] << std::endl;
    }

  filter->SetImage( image );
  filter->SetNumberOfClasses( numberOfClasses );
  filter->SetClassLabels( classLabelsObject );
  filter->SetDecisionRule( decisionRule );
  filter->SetMembershipFunctions( membershipFunctionsObject );
  filter->SetMembershipFunctionsWeightsArray( weightArrayObjects );

  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  //Check if the measurement vectors are correctly labelled. 
  //TODO

  std::cerr << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}

