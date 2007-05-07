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
  REGISTER_TEST(itkSampleToHistogramFilterTest);
  REGISTER_TEST(itkSampleToHistogramFilterTest2);
  REGISTER_TEST(itkSampleToHistogramFilterTest3);
  REGISTER_TEST(itkSampleToHistogramFilterTest4);
  REGISTER_TEST(itkSampleToHistogramFilterTest5);
  REGISTER_TEST(itkSampleToHistogramFilterTest6);
  REGISTER_TEST(itkMeanFilterTest);
  REGISTER_TEST(itkMembershipSampleTest1);
  REGISTER_TEST(itkMembershipSampleTest2);
  REGISTER_TEST(itkMembershipSampleTest3);
  REGISTER_TEST(itkNeighborhoodSamplerTest1);
  REGISTER_TEST(itkCovarianceFilterTest);
  REGISTER_TEST(itkCovarianceFilterTest2);
  REGISTER_TEST(itkCovarianceFilterTest3);
  REGISTER_TEST(itkMeasurementVectorTraitsTest);
  REGISTER_TEST(itkScalarImageToCooccurrenceMatrixFilterTest);
  REGISTER_TEST(itkScalarImageToCooccurrenceMatrixFilterTest2);
  REGISTER_TEST(itkSampleTest);
  REGISTER_TEST(itkSampleTest2);
  REGISTER_TEST(itkSampleTest3);
  REGISTER_TEST(itkSampleTest4);
  REGISTER_TEST(itkSampleClassifierFilterTest1);
  REGISTER_TEST(itkSampleToSubsampleFilterTest1);
  REGISTER_TEST(itkSubsampleTest);
  REGISTER_TEST(itkSubsampleTest2);
  REGISTER_TEST(itkSubsampleTest3);
  REGISTER_TEST(itkStatisticsAlgorithmTest);
  REGISTER_TEST(itkWeightedMeanFilterTest);
  REGISTER_TEST(itkWeightedCovarianceFilterTest);
  REGISTER_TEST(itkImageToListSampleAdaptorTest); 
  REGISTER_TEST(itkImageToListSampleAdaptorTest2); 
  REGISTER_TEST(itkPointSetToListSampleAdaptorTest); 
  REGISTER_TEST(itkJointDomainImageToListSampleAdaptorTest); 
}

