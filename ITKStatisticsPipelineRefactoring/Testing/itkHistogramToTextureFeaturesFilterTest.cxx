/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHistogramToTextureFeaturesFilterTest.cxx,v $
  Language:  C++
  Date:      $Date: 2004/06/25 11:54:23 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
// Insight classes
#include "itkHistogram.h"
#include "vnl/vnl_math.h"

#include "itkHistogramToTextureFeaturesFilter.h"

int itkHistogramToTextureFeaturesFilterTest(int, char* [] )
{
  //Data definitions 
  const unsigned int  HISTOGRAM_AXIS_LEN =  25;


  //------------------------------------------------------
  // Create a simple test histogram. The histogram must be
  // symmetric and normalized.
  //------------------------------------------------------
  typedef float MeasurementType ;
  typedef itk::Statistics::Histogram< MeasurementType, 2 > HistogramType ;
  typedef itk::Statistics::HistogramToTextureFeaturesFilter<
    HistogramType > filter;
  filter::Pointer glcmCalc = filter::New();
  

  
}

