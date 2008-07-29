/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkListSampleTest.cxx,v $
Language:  C++
Date:      $Date: 2007/04/06 15:26:57 $
Version:   $Revision: 1.12 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkArray.h"
#include "itkVariableLengthVector.h"
#include "itkListSample.h"
#include "itkSampleClassifierFilter.h"

// ADD DistanceToCentroidMembershipFunction (with the added SetDistanceMetric() method
// ADD EuclideanDistanceMetri
// Create two classes with their respective DistanceToCentroidMembershipFunction and two separate centroids
// ADD MinimumDecisionRule
// Run that classification.  

int itkSampleClassifierFilterTest1(int argc, char *argv[] )
{

  const unsigned int numberOfComponents = 3;
  typedef float      MeasurementType;

  const unsigned int numberOfClasses = 2;

  typedef itk::Array< MeasurementType > MeasurementVectorType;
  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;

  typedef itk::Statistics::SampleClassifierFilter< SampleType > FilterType;

  FilterType::Pointer filter = FilterType::New();

  SampleType::Pointer sample = SampleType::New();

  // Test GetInput() before setting the input
  if( filter->GetInput() != NULL )
    {
    std::cerr << "GetInput() should have returned NULL" << std::endl;
    return EXIT_FAILURE;
    }

  // Test GetOutput() before creating the output
  if( filter->GetOutput() == NULL )
    {
    std::cerr << "GetOutput() should have returned NON-NULL" << std::endl;
    return EXIT_FAILURE;
    }

  filter->SetInput( sample );

  if( filter->GetInput() != sample.GetPointer() )
    {
    std::cerr << "GetInput() didn't matched SetInput()" << std::endl;
    return EXIT_FAILURE;
    }

  filter->SetNumberOfClasses( numberOfClasses );

  if( filter->GetNumberOfClasses() != numberOfClasses )
    {
    std::cerr << "GetNumberOfClasses() didn't matched SetNumberOfClasses()" << std::endl;
    return EXIT_FAILURE;
    }

  typedef FilterType::ClassLabelVectorObjectType               ClassLabelVectorType;
  typedef FilterType::MembershipFunctionVectorObjectType       MembershipFunctionVectorType;

  ClassLabelVectorType::Pointer  classLabels = ClassLabelVectorType::New();

  MembershipFunctionVectorType::Pointer  membershipFunctions = MembershipFunctionVectorType::New();

  filter->SetClassLabels( classLabels );

  filter->SetMembershipFunctions( membershipFunctions );

  // Exercise the Print method.
  filter->Print( std::cout );

  sample->SetMeasurementVectorSize( numberOfComponents );


  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  // Test GetOutput() after creating the output
  if( filter->GetOutput() == NULL )
    {
    std::cerr << "GetOutput() should have returned NON-NULL" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}



