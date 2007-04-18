/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkStatisticsPrintTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/08/24 15:16:21 $
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

#include "itkFixedArray.h"
#include "itkImage.h"
#include "itkPointSet.h"

// #include "itkSample.h"   // abstract class
#include "itkHistogram.h"
#include "itkListSample.h"
#include "itkSubsample.h"
#include "itkSampleClassifierFilter.h"
#include "itkListSampleToHistogramFilter.h"
#include "itkMembershipSample.h"
#include "itkImageToListSampleFilter.h"
#include "itkScalarImageToCooccurrenceMatrixFilter.h"
#include "itkHistogramToTextureFeaturesFilter.h"

int itkStatisticsPrintTest(int , char* [])
{
  typedef float MeasurementType;
  typedef float FrequencyType;
  typedef itk::FixedArray< MeasurementType, 2 >  MeasurementVectorType;
  typedef itk::Image< MeasurementVectorType, 3 > ImageType;
  typedef itk::Image< unsigned char, 3>          ScalarImageType;
  typedef itk::PointSet< MeasurementType >       PointSetType;

  typedef itk::Statistics::ListSample< MeasurementVectorType >
    SampleType;

  typedef itk::Statistics::Subsample< SampleType >
    SubSampleType;

  typedef itk::Statistics::Histogram< MeasurementType, 2 > HistogramType ;

  typedef itk::Statistics::ListSampleToHistogramFilter< 
    SampleType, HistogramType > ListSampleToHistogramFilterType;

  typedef itk::Statistics::SampleClassifierFilter< 
    SampleType > SampleClassifierFilterType;

  typedef itk::Statistics::ImageToListSampleFilter< 
    ImageType, ImageType > ImageToListSampleFilterType;

  typedef itk::Statistics::ScalarImageToCooccurrenceMatrixFilter< 
    ScalarImageType > ScalarImageToCooccurrenceMatrixFilterType;

  typedef itk::Statistics::MembershipSample< SampleType >
    MembershipSampleType;

  typedef itk::Statistics::HistogramToTextureFeaturesFilter<
    HistogramType > HistogramToTextureFeaturesFilterType;

  SampleType::Pointer sampleObj = SampleType::New();
  std::cout << "----------ListSample " << sampleObj;

  SubSampleType::Pointer subsampleObj = SubSampleType::New();
  std::cout << "----------Subsample " << subsampleObj;

  HistogramType::Pointer HistogramObj=
    HistogramType::New();
  std::cout << "----------Histogram " << HistogramObj;

  ListSampleToHistogramFilterType::Pointer ListSampleToHistogramFilterObj =
    ListSampleToHistogramFilterType::New();
  std::cout << "----------ListSampleToHistogramFilter ";
  std::cout << ListSampleToHistogramFilterObj;

  SampleClassifierFilterType::Pointer SampleClassifierFilterObj =
    SampleClassifierFilterType::New();
  std::cout << "----------SampleClassifierFilter ";
  std::cout << SampleClassifierFilterObj;

  ImageToListSampleFilterType::Pointer ImageToListSampleFilterObj =
    ImageToListSampleFilterType::New();
  std::cout << "----------ImageToListSampleFilter ";
  std::cout << ImageToListSampleFilterObj;

  ScalarImageToCooccurrenceMatrixFilterType::Pointer ScalarImageToCooccurrenceMatrixFilterObj =
    ScalarImageToCooccurrenceMatrixFilterType::New();
  std::cout << "----------ScalarImageToCooccurrenceMatrixFilter ";
  std::cout << ScalarImageToCooccurrenceMatrixFilterObj;

  HistogramToTextureFeaturesFilterType::Pointer HistogramToTextureFeaturesFilterObj=
    HistogramToTextureFeaturesFilterType::New();
  std::cout << "----------HistogramToTextureFeaturesFilter " << HistogramToTextureFeaturesFilterObj;

  MembershipSampleType::Pointer MembershipSampleObj = 
    MembershipSampleType::New();
  std::cout << "----------MembershipSample " << MembershipSampleObj;

  return EXIT_SUCCESS;
}
