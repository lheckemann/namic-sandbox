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
#include "itkFixedArray.h"

#define itkSetGetLengthVerificationMacro( measure, len1, len2 ) \
  itk::MeasurementVectorTraits::SetLength( measure1, len1 ); \
  if( itk::MeasurementVectorTraits::GetLength( measure1 ) != len2 ) \
    { \
    std::cerr << "Set/GetLength() failed in measure " << std::endl; \
    } 

#define itkSetLengthExceptionMacro( measure, len ) \
  try \
    { \
    itk::MeasurementVectorTraits::SetLength( measure, len ); \
    std::cerr << "Failed to get expected exception for SetLength() "; \
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

  const unsigned int length2 = 19;

  MeasurementVectorType1 measure1;
  MeasurementVectorType2 measure2;

  itkSetGetLengthVerificationMacro( measure1    , length1, length1 );
  itkSetGetLengthVerificationMacro( (&measure1) , length1, length1 );

  itkSetLengthExceptionMacro( measure1    , length2 );
  itkSetLengthExceptionMacro( (&measure1) , length2 );

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}
