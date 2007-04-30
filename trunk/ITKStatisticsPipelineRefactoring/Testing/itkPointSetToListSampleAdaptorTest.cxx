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
   
  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}







