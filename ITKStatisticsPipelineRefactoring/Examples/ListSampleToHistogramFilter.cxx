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

int main(int argc, char *argv[] ) 
{
    
  const unsigned int numberOfComponents = 3;
  typedef float      MeasurementType;

  typedef itk::Array< MeasurementType > MeasurementVectorType;
  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;

  typedef itk::Statistics::Histogram< MeasurementType, 
          numberOfComponents, 
          itk::Statistics::DenseFrequencyContainer > HistogramType;

  typedef itk::Statistics::ListSampleToHistogramFilter< 
    SampleType, HistogramType > FilterType;

  typedef FilterType::HistogramSizeType                    HistogramSizeType;
  typedef FilterType::HistogramMeasurementType             HistogramMeasurementType;

  FilterType::Pointer filter = FilterType::New();  

  SampleType::Pointer sample = SampleType::New();


  filter->SetInput( sample );

  filter->SetMarginalScale( 50 );

  HistogramSizeType histogramSize;
  histogramSize[0] = 256;
  histogramSize[1] = 256;
  histogramSize[2] = 256;

  filter->SetHistogramSize( histogramSize );

  return EXIT_SUCCESS;
}



