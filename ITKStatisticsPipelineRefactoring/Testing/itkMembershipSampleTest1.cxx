/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/04/11 13:52:36 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <iostream>
#include "itkListSample.h"
#include "itkMembershipSample.h"

int itkMembershipSampleTest1(int, char* [] )
{

  const unsigned int MeasurementVectorSize = 3;

  const unsigned int numberOfClasses1 = 2;
  const unsigned int numberOfClasses2 = 333;

  typedef itk::FixedArray< 
    float, MeasurementVectorSize >  MeasurementVectorType;

  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;

  typedef itk::Statistics::MembershipSample< SampleType >   MembershipSampleType;

  SampleType::Pointer sample = SampleType::New();


  MembershipSampleType::Pointer membershipSample = MembershipSampleType::New();

  membershipSample->SetSample( sample );

  if( membershipSample->GetSample() != sample )
    {
    std::cerr << "SetSample() / GetSample() failed " << std::endl;
    return EXIT_FAILURE;
    }

  membershipSample->SetNumberOfClasses( numberOfClasses1 );
  if( membershipSample->GetNumberOfClasses() != numberOfClasses1 )
    {
    std::cerr << "SetNumberOfClasses() / GetNumberOfClasses() 1 failed " << std::endl;
    return EXIT_FAILURE;
    }

  // Check that it can be changed...
  membershipSample->SetNumberOfClasses( numberOfClasses2 );
  if( membershipSample->GetNumberOfClasses() != numberOfClasses2 )
    {
    std::cerr << "SetNumberOfClasses() / GetNumberOfClasses() 2 failed " << std::endl;
    return EXIT_FAILURE;
    }
 

  //
  // Exercise the Print() method
  //
  membershipSample->Print( std::cout );

  std::cout << "Test Passed !" << std::endl;
  return EXIT_SUCCESS;
}
