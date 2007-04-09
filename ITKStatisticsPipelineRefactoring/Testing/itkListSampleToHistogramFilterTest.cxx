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
#include "itkHistogram.h"
#include "itkListSampleToHistogramFilter.h"

int itkListSampleToHistogramFilterTest(int argc, char *argv[] ) 
{
    
  typedef itk::Array< float > MeasurementVectorType;
  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;
    
  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}



