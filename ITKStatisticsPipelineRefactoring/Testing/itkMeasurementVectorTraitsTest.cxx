/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHistogramTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/08/24 15:16:13 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkMeasurementVectorTraits.h"

#define itkSetGetLengthVerificationMacro( measure, len1, len2 ) \
  itk::MeasurementVectorTraits::SetLength( (measure), len1 ); \
  if( itk::MeasurementVectorTraits::GetLength( (measure) ) != len2 ) \
    { \
    std::cerr << "Set/GetLength() failed in measure " << std::endl; \
    } 

#define itkSetLengthExceptionMacro( measure, len ) \
  try \
    { \
    itk::MeasurementVectorTraits::SetLength( (measure), len ); \
    std::cerr << "Failed to get expected exception for SetLength() "; \
    std::cerr << std::endl; \
    return EXIT_FAILURE; \
    } \
  catch( itk::ExceptionObject & )  \
    { \
    } 

#define itkAssertLengthExceptionMacro( m1, m2 ) \
  try \
    { \
    itk::MeasurementVectorTraits::Assert( (m1), (m2) ); \
    std::cerr << "Failed to get expected exception for Assert() "; \
    std::cerr << std::endl; \
    return EXIT_FAILURE; \
    } \
  catch( itk::ExceptionObject & )  \
    { \
    } 


int itkMeasurementVectorTraitsTest(int, char* [] ) 
{
  std::cout << "MeasurementVectorTraits Test" << std::endl; 
  
  const unsigned int length1 = 7;

  typedef itk::FixedArray< float, length1 >   MeasurementVectorType1;
  typedef itk::Array< float >                 MeasurementVectorType2;
  typedef itk::VariableLengthVector< float >  MeasurementVectorType3;
  typedef std::vector< float >                MeasurementVectorType4;

  const unsigned int length2 = 19;

  typedef itk::FixedArray< float, length2 >   MeasurementVectorType1b;
  typedef itk::Array< float >                 MeasurementVectorType2b;
  typedef itk::VariableLengthVector< float >  MeasurementVectorType3b;
  typedef std::vector< float >                MeasurementVectorType4b;


  MeasurementVectorType1 measure1;
  MeasurementVectorType2 measure2;
  MeasurementVectorType3 measure3;
  MeasurementVectorType4 measure4;

  itkSetGetLengthVerificationMacro( measure1    , length1, length1 );
  itkSetGetLengthVerificationMacro( &measure1   , length1, length1 );
  itkSetLengthExceptionMacro( measure1    , length2 );
  itkSetLengthExceptionMacro( &measure1   , length2 );

  itkSetGetLengthVerificationMacro( measure2  , length1, length1 );
  itkSetGetLengthVerificationMacro( measure2  , length2, length2 );
  itkSetGetLengthVerificationMacro( &measure2 , length1, length1 );
  itkSetGetLengthVerificationMacro( &measure2 , length2, length2 );

  itkSetGetLengthVerificationMacro( measure3  , length1, length1 );
  itkSetGetLengthVerificationMacro( measure3  , length2, length2 );
  itkSetGetLengthVerificationMacro( &measure3 , length1, length1 );
  itkSetGetLengthVerificationMacro( &measure3 , length2, length2 );

  itkSetGetLengthVerificationMacro( measure4  , length1, length1 );
  itkSetGetLengthVerificationMacro( measure4  , length2, length2 );
  itkSetGetLengthVerificationMacro( &measure4 , length1, length1 );
  itkSetGetLengthVerificationMacro( &measure4 , length2, length2 );

  // Test the Asser() methods
  //
  MeasurementVectorType1b measure1b;
  MeasurementVectorType2b measure2b;
  MeasurementVectorType3b measure3b;
  MeasurementVectorType4b measure4b;

  itk::MeasurementVectorTraits::SetLength( measure1b, length2 );
  itk::MeasurementVectorTraits::SetLength( measure2b, length2 );
  itk::MeasurementVectorTraits::SetLength( measure3b, length2 );
  itk::MeasurementVectorTraits::SetLength( measure4b, length2 );

  itkAssertLengthExceptionMacro( measure1, measure2b );
  itkAssertLengthExceptionMacro( measure1, measure3b );

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}
