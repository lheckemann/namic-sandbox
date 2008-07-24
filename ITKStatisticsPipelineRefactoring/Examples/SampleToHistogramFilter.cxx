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
#include "itkSampleToHistogramFilter.h"

int main(int argc, char *argv[] ) 
{
    
  typedef float      MeasurementType;

  typedef itk::Array< MeasurementType > MeasurementVectorType;
  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;

  typedef itk::Statistics::Histogram< MeasurementType, 
          itk::Statistics::DenseFrequencyContainer2 > HistogramType;

  typedef itk::Statistics::SampleToHistogramFilter< 
    SampleType, HistogramType > FilterType;

  typedef FilterType::HistogramSizeType                    HistogramSizeType;
  typedef FilterType::HistogramMeasurementType             HistogramMeasurementType;
  typedef HistogramType::AbsoluteFrequencyType                     AbsoluteFrequencyType;
  typedef HistogramType::InstanceIdentifier                InstanceIdentifier;
  typedef HistogramType::MeasurementVectorType             HistogramMeasurementVectorType;


  FilterType::Pointer filter = FilterType::New();  

  SampleType::Pointer sample = SampleType::New();


  filter->SetInput( sample );

  filter->SetMarginalScale( 50 );

  HistogramSizeType histogramSize;
  histogramSize[0] = 256;
  histogramSize[1] = 256;
  histogramSize[2] = 256;

  filter->SetHistogramSize( histogramSize );


  const HistogramType * histogram = filter->GetOutput();

  HistogramType::ConstIterator itr = histogram->Begin();
  HistogramType::ConstIterator end = histogram->End();


  while( itr != end )
    {
    const AbsoluteFrequencyType frequency = itr.GetFrequency();
    const InstanceIdentifier identifier = itr.GetInstanceIdentifier();
    const HistogramMeasurementVectorType vector = itr.GetMeasurementVector();
    std::cout << identifier << " ";
    std::cout << frequency << " ";
    std::cout << vector << std::endl;
    ++itr;
    }


  return EXIT_SUCCESS;
}



