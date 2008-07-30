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

  // Exercise the Print() method
  membershipSample->Print( std::cout );

  // Add measurmement vectors to the list sample
  unsigned int sampleSize = 10;
  MeasurementVectorType mv;

  std::cout << "Sample length = " << sample->GetMeasurementVectorSize() << std::endl;
  std::cout << "Vector length = " << itk::Statistics::MeasurementVectorTraits::GetLength( mv ) << std::endl;

  for ( unsigned int i = 0; i < sampleSize; i++ )
    {
    for (unsigned int j = 0; j < MeasurementVectorSize; j++ )
      {
      mv[j] = rand() / (RAND_MAX+1.0) ;
      }
    sample->PushBack(mv);
    }

  // Add instances to the membership sample
  SampleType::ConstIterator   begin = sample->Begin();
  SampleType::ConstIterator   end   = sample->End();

  SampleType::ConstIterator iter= begin;

  MembershipSampleType::ClassLabelType classLabel = 0;

  unsigned int sampleCounter = 0;
  
  while( iter != end ) 
    {
    if( sampleCounter < 5 ) 
      {
      classLabel = 1;
      }
    SampleType::InstanceIdentifier id = iter.GetInstanceIdentifier();  
    membershipSample->AddInstance( classLabel, id );
    ++iter;
    ++sampleCounter;
    }
    
  std::cout << "Test Passed !" << std::endl;
  return EXIT_SUCCESS;
}
