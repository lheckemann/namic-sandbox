/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkStatisticsTypesTest.cxx,v $
  Language:  C++
  Date:      $Date: 2009-05-02 14:56:00 $
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

#include "itkMeasurementVectorTraits.h" 

#define declareType( _x ) \
  typedef itk::Statistics::MeasurementVectorTraits::_x _x; \
  std::cout << #_x << " = " << sizeof( _x ) << " bytes "; \
  if( itk::NumericTraits< _x >::is_integer ) \
    { \
    std::cout << " Integer type " << std::endl; \
    } \
  else \
    {  \
    std::cout << " Real type " << std::endl; \
    } 


int itkStatisticsTypesTest(int, char * [])
{

  declareType( InstanceIdentifier );
  declareType( AbsoluteFrequencyType );
  declareType( RelativeFrequencyType );
  declareType( TotalAbsoluteFrequencyType );
  declareType( TotalRelativeFrequencyType );

  return EXIT_SUCCESS;
}
