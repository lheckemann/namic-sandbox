/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkExpectationMaximizationMixtureModelEstimatorTest.cxx,v $
Language:  C++
Date:      $Date: 2005/02/08 03:18:41 $
Version:   $Revision: 1.8 $

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
#include "itkVector.h"
#include "itkPointSetToListSampleAdaptor.h"

int itkPointSetToListSampleAdaptorTest(int argc, char* argv[] )
{
  typedef itk::PointSet< double, 3 >                                   PointSetType ;
  typedef itk::Statistics::PointSetToListSampleAdaptor< PointSetType > PointSetToListSampleAdaptorType;

  PointSetType::Pointer pointSet = PointSetType::New() ;
  PointSetType::PointType point;

  unsigned int numberOfPoints=10;
  for( unsigned int i=0; i < numberOfPoints; i++ )
    {
    point[0] = i*3;
    point[1] = i*3 + 1;
    point[2] = i*3 + 2;
    pointSet->SetPoint( i, point );
    }  

  PointSetToListSampleAdaptorType::Pointer  listSample = PointSetToListSampleAdaptorType::New(); 
  
  //Test if the methods throw exceptions if invoked before setting the pointset
  try
    {
    unsigned long size = listSample->Size();
    std::cerr << "Exception should have been thrown since the input point set  \
                  is not set yet" << std::endl;
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Caught expected exception: " << excp << std::endl;
    }
  try
    {
    PointSetToListSampleAdaptorType::FrequencyType totalFrequency = listSample->GetTotalFrequency();
    std::cerr << "Exception should have been thrown since the input point set  \
                  is not set yet" << std::endl;
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Caught expected exception: " << excp << std::endl;
    }

  try
    {
    PointSetToListSampleAdaptorType::MeasurementVectorType m = listSample->GetMeasurementVector( 0 );
    std::cerr << "Exception should have been thrown since the input point set  \
                  is not set yet" << std::endl;
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Caught expected exception: " << excp << std::endl;
    }

  try
    {
    const PointSetToListSampleAdaptorType::PointSetType * set = listSample->GetPointSet( );
    std::cerr << "Exception should have been thrown since the input point set  \
                  is not set yet" << std::endl;
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Caught expected exception: " << excp << std::endl;
    }
 
  try
    {
    PointSetToListSampleAdaptorType::FrequencyType frequency = listSample->GetFrequency(0 );
    std::cerr << "Exception should have been thrown since the input point set  \
                  is not set yet" << std::endl;
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Caught expected exception: " << excp << std::endl;
    }
 

  listSample->SetPointSet( pointSet.GetPointer() ) ;

  //exercise returned pointset
  const PointSetToListSampleAdaptorType::PointSetType * pointSetReturned = listSample->GetPointSet( );
   
  //check size
  if (numberOfPoints != listSample->Size())
    {
    std::cerr << "Size() is not returning the correct size"<< std::endl;
    return EXIT_FAILURE;
    }

  //check frequency
  if ( listSample->GetFrequency( 0 ) != 1 )
    {
    std::cerr << "GetFrequency() is not returning the correct frequency"<< std::endl;
    return EXIT_FAILURE;
    }

  //check frequency
  if (numberOfPoints != listSample->GetTotalFrequency())
    {
    std::cerr << "GetTotalFrequency() is not returning the correct frequency"<< std::endl;
    return EXIT_FAILURE;
    }


  listSample->Print( std::cout );

  for( unsigned int i=0; i < numberOfPoints; i++ )
    {
    PointSetToListSampleAdaptorType::InstanceIdentifier id = i;
    PointSetType::PointType   tempPointSet;
    pointSet->GetPoint( i, &tempPointSet );
        
    if ( listSample->GetMeasurementVector( id ) != tempPointSet )
      {
      std::cerr << "Error in point set accessed by the adaptor" << std::endl;
      return EXIT_FAILURE;
      }
    }
   
  //Test the iterators
  std::cerr << "Iterators..." << std::endl;
  {
  // forward iterator
  typedef PointSetToListSampleAdaptorType::Iterator IteratorType;
  
  IteratorType s_iter = listSample->Begin();
  
  // copy constructor
  IteratorType bs_iter(s_iter);
  if (bs_iter != s_iter)
    {
    std::cerr << "Iterator::Copy Constructor failed" << std::endl;
    return EXIT_FAILURE;    
    }
  
  // assignment operator 
  IteratorType assignment_iter( bs_iter );
  assignment_iter = s_iter;
  if (assignment_iter != s_iter)
    {
    std::cerr << "Iterator::assignment operator failed" << std::endl;
    return EXIT_FAILURE;    
    }

  PointSetToListSampleAdaptorType::InstanceIdentifier id = 0;
  while (s_iter != listSample->End())
    {
    if (listSample->GetMeasurementVector(id) != 
        s_iter.GetMeasurementVector())
      {
      std::cerr << "Iterator::GetMeasurementVector (forward) failed" 
                << std::endl;
      return EXIT_FAILURE;
      }
    if (id != s_iter.GetInstanceIdentifier())
      {
      std::cerr << "Iterator::GetInstanceIdentifier (forward) failed" 
                << std::endl;
      return EXIT_FAILURE;
      }
    if (s_iter.GetFrequency() != 1)
      {
      std::cerr << "Iterator::GetFrequency (forward) failed" << std::endl;
      return EXIT_FAILURE;
      }
    if (listSample->GetFrequency(id) != 1)
      {
      std::cerr << "GetFrequency (forward) failed" << std::endl;
      return EXIT_FAILURE;
      }
    ++id;
    ++s_iter;
    }
  
  if (s_iter != listSample->End())
    {
    std::cerr << "Iterator::End (forward) failed" << std::endl;
    return EXIT_FAILURE;    
    }
  
  }

  std::cerr << "Const Iterators..." << std::endl;
  {
  // forward iterator
  typedef PointSetToListSampleAdaptorType::ConstIterator  ConstIteratorType;
  
  ConstIteratorType s_iter = listSample->Begin();
  
  // copy constructor
  ConstIteratorType bs_iter(s_iter);
  if (bs_iter != s_iter)
    {
    std::cerr << "Iterator::Copy Constructor (from const) failed" 
              << std::endl;
    return EXIT_FAILURE;    
    }

  // assignment operator
  ConstIteratorType assignment_iter( bs_iter );
  assignment_iter = s_iter;
  if (assignment_iter != s_iter)
    {
    std::cerr << "Const Iterator::operator= () failed" 
              << std::endl;
    return EXIT_FAILURE;    
    }

  // copy from non-const iterator
  PointSetToListSampleAdaptorType::Iterator nonconst_iter = listSample->Begin();
  PointSetToListSampleAdaptorType::ConstIterator s2_iter(nonconst_iter);
  if (s2_iter != s_iter)
    {
    std::cerr << "Iterator::Copy Constructor (from non-const) failed" 
              << std::endl;
    return EXIT_FAILURE;    
    }
   
  // assignment from non-const iterator
  s2_iter = nonconst_iter;
  if (s2_iter != s_iter)
    {
    std::cerr << "Iterator::assignment (from non-const) failed" << std::endl;
    return EXIT_FAILURE;    
    }
  
  PointSetToListSampleAdaptorType::InstanceIdentifier id = 0;
  while (s_iter != listSample->End())
    {
    if (listSample->GetMeasurementVector(id) != 
        s_iter.GetMeasurementVector())
      {
      std::cerr << "Iterator::GetMeasurementVector (forward) failed" 
                << std::endl;
      return EXIT_FAILURE;
      }
    if (id != s_iter.GetInstanceIdentifier())
      {
      std::cerr << "Iterator::GetInstanceIdentifier (forward) failed" 
                << std::endl;
      return EXIT_FAILURE;
      }
    if (s_iter.GetFrequency() != 1)
      {
      std::cerr << "Iterator::GetFrequency (forward) failed" << std::endl;
      return EXIT_FAILURE;
      }
    ++id;
    ++s_iter;
    }
  
  if (s_iter != listSample->End())
    {
    std::cerr << "Iterator::End (forward) failed" << std::endl;
    return EXIT_FAILURE;    
    }
  
  }



  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}







