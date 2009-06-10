/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkStatisticsTests.cxx,v $
  Language:  C++
  Date:      $Date: 2008-04-29 22:56:43 $
  Version:   $Revision: 1.36 $

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
  REGISTER_TEST(itkChiSquareDistributionTest);
  REGISTER_TEST(itkGaussianDistributionTest);
  REGISTER_TEST(itkTDistributionTest);
  REGISTER_TEST(itkCovarianceCalculatorTest);
  REGISTER_TEST(itkDenseFrequencyContainerTest);
  REGISTER_TEST(itkExpectationMaximizationMixtureModelEstimatorTest);
  REGISTER_TEST(itkGaussianDensityFunctionTest);
  REGISTER_TEST(itkGoodnessOfFitMixtureModelCostFunctionTest);
  REGISTER_TEST(itkGreyLevelCooccurrenceMatrixTextureCoefficientsCalculatorTest);
  REGISTER_TEST(itkHistogramTest);
  REGISTER_TEST(itkImageToListAdaptorTest);
  REGISTER_TEST(itkImageToCooccurrenceListAdaptorTest);
  REGISTER_TEST(itkImageToHistogramGeneratorTest);
  REGISTER_TEST(itkImageToListGeneratorTest);
  REGISTER_TEST(itkKdTreeBasedKmeansEstimatorTest);
  REGISTER_TEST(itkKdTreeGeneratorTest);
  REGISTER_TEST(itkKdTreeTest1);
  REGISTER_TEST(itkKdTreeTest2);
  REGISTER_TEST(itkListSampleTest);
  REGISTER_TEST(itkListSampleToHistogramFilterTest);
  REGISTER_TEST(itkListSampleToHistogramGeneratorTest);
  REGISTER_TEST(itkMaskedScalarImageToGreyLevelCooccurrenceMatrixGeneratorTest);
  REGISTER_TEST(itkMembershipSampleTest);
  REGISTER_TEST(itkMembershipSampleGeneratorTest);
  REGISTER_TEST(itkMersenneTwisterRandomVariateGeneratorTest);
  REGISTER_TEST(itkMeanCalculatorTest);
  REGISTER_TEST(itkNeighborhoodSamplerTest) ;
  REGISTER_TEST(itkNormalVariateGeneratorTest);
  REGISTER_TEST(itkNthElementTest1);
  REGISTER_TEST(itkQuickSelectTest1);
  REGISTER_TEST(itkSampleClassifierTest) ;
  REGISTER_TEST(itkSampleClassifierWithMaskTest) ;
  REGISTER_TEST(itkSampleMeanShiftClusteringFilterTest) ;
  REGISTER_TEST(itkSampleSelectiveMeanShiftBlurringFilterTest) ;
  REGISTER_TEST(itkSelectiveSubsampleGeneratorTest) ;
  REGISTER_TEST(itkScalarImageToHistogramGeneratorTest);
  REGISTER_TEST(itkScalarImageTextureCalculatorTest);
  REGISTER_TEST(itkScalarImageToGreyLevelCooccurrenceMatrixGeneratorTest);
  REGISTER_TEST(itkStatisticsAlgorithmTest);
  REGISTER_TEST(itkSubsampleTest);
  REGISTER_TEST(itkVariableDimensionHistogramTest);
  REGISTER_TEST(itkWeightedCentroidKdTreeGeneratorTest1);
  REGISTER_TEST(itkWeightedCovarianceCalculatorTest);
  REGISTER_TEST(itkWeightedMeanCalculatorTest);
}

