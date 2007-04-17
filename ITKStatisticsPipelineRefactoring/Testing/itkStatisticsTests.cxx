/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkStatisticsTests.cxx,v $
  Language:  C++
  Date:      $Date: 2007/02/24 17:53:01 $
  Version:   $Revision: 1.31 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

// this file defines the itkStatisticsTest for the test driver
// and all it expects is that you have a function called RegisterTests
#include <iostream>
#include "vnl/vnl_sample.h"
#include "itkTestMain.h" 


void RegisterTests()
{
  vnl_sample_reseed(8775070);
  REGISTER_TEST(itkStatisticsPrintTest);
  REGISTER_TEST(itkHistogramTest);
  REGISTER_TEST(itkHistogramToTextureFeaturesFilterTest);
  REGISTER_TEST(itkImageToListSampleFilterTest);
  REGISTER_TEST(itkImageToListSampleFilterTest2);
  REGISTER_TEST(itkImageToListSampleFilterTest3);
  REGISTER_TEST(itkListSampleTest);
  REGISTER_TEST(itkListSampleToHistogramFilterTest);
  REGISTER_TEST(itkMeanFilterTest);
  REGISTER_TEST(itkMeasurementVectorTraitsTest);
  REGISTER_TEST(itkScalarImageToCooccurrenceMatrixFilterTest);
  REGISTER_TEST(itkScalarImageToCooccurrenceMatrixFilterTest2);
  REGISTER_TEST(itkSampleTest);
  REGISTER_TEST(itkSubsampleTest);
  REGISTER_TEST(itkSubsampleTest2);
  REGISTER_TEST(itkSubsampleTest3);
  REGISTER_TEST(itkStatisticsAlgorithmTest);
  REGISTER_TEST(itkWeightedMeanFilterTest);
}

