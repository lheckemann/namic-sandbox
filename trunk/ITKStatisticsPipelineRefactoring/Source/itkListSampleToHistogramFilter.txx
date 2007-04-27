/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSample.h,v $
  Language:  C++
  Date:      $Date: 2006/02/21 18:53:18 $
  Version:   $Revision: 1.26 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkListSampleToHistogramFilter_txx
#define __itkListSampleToHistogramFilter_txx

#include "itkListSampleToHistogramFilter.h"
#include "itkStatisticsAlgorithm.h"

namespace itk { 
namespace Statistics {

template < class TSample, class THistogram >
ListSampleToHistogramFilter< TSample, THistogram >
::ListSampleToHistogramFilter()
{
  this->ProcessObject::SetNumberOfRequiredInputs( 1 );

  // Create the output. We use static_cast<> here because we know the default
  // output must be of type HistogramType
  typename HistogramType::Pointer output
    = static_cast<HistogramType*>(this->MakeOutput(0).GetPointer()); 
  this->ProcessObject::SetNumberOfRequiredOutputs(1);
  this->ProcessObject::SetNthOutput(0, output.GetPointer());

  // Set some default inputs 
  HistogramSizeType histogramSize;
  histogramSize.Fill(0);
  this->SetHistogramSize( histogramSize );

  this->SetMarginalScale( 100 );

  this->SetAutoMinimumMaximum( true );
}

template < class TSample, class THistogram >
ListSampleToHistogramFilter< TSample, THistogram >
::~ListSampleToHistogramFilter()
{
}

template < class TSample, class THistogram >
void
ListSampleToHistogramFilter< TSample, THistogram >
::SetInput( const SampleType * sample )
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(0, 
                                   const_cast< SampleType * >( sample ) );
}

template < class TSample, class THistogram >
const typename 
ListSampleToHistogramFilter< TSample, THistogram >::SampleType *
ListSampleToHistogramFilter< TSample, THistogram >
::GetInput() const
{
  const SampleType * input = 
    static_cast< const SampleType * >( this->ProcessObject::GetInput( 0 ) );
  return input;
}


template < class TSample, class THistogram >
const typename 
ListSampleToHistogramFilter< TSample, THistogram >::HistogramType *
ListSampleToHistogramFilter< TSample, THistogram >
::GetOutput() const
{
  const HistogramType * output = 
    static_cast<const HistogramType*>(this->ProcessObject::GetOutput(0));

  return output;
}


template < class TSample, class THistogram >
typename ListSampleToHistogramFilter< TSample, THistogram >::DataObjectPointer
ListSampleToHistogramFilter< TSample, THistogram >
::MakeOutput(unsigned int)
{
  return static_cast<DataObject*>(HistogramType::New().GetPointer());
}


template < class TSample, class THistogram >
void
ListSampleToHistogramFilter< TSample, THistogram >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}


template < class TSample, class THistogram >
void
ListSampleToHistogramFilter< TSample, THistogram >
::GenerateData()
{
  const SampleType * inputSample = this->GetInput();

  const InputHistogramMeasurementVectorObjectType * binMinimumObject =
    this->GetHistogramBinMinimumInput();

  const InputHistogramMeasurementVectorObjectType * binMaximumObject =
    this->GetHistogramBinMaximumInput();

  const InputHistogramMeasurementObjectType * marginalScaleObject =
    this->GetMarginalScaleInput();
 
  const InputBooleanObjectType * autoMinimumMaximum =
    this->GetAutoMinimumMaximumInput();

  const InputHistogramSizeObjectType * histogramSizeObject =
    this->GetHistogramSizeInput();
    
  if( histogramSizeObject == NULL )
    {
    itkExceptionMacro("Histogram Size input is missing");
    }
    
  if( marginalScaleObject == NULL )
    {
    itkExceptionMacro("Marginal scale input is missing");
    }

  HistogramSizeType histogramSize = histogramSizeObject->Get();

  MeasurementType marginalScale = marginalScaleObject->Get();

  HistogramType * outputHistogram = 
    static_cast<HistogramType*>(this->ProcessObject::GetOutput(0));

  const typename SampleType::InstanceIdentifier measurementVectorSize =
    inputSample->GetMeasurementVectorSize();

  typename SampleType::MeasurementVectorType lower;
  typename SampleType::MeasurementVectorType upper;

  MeasurementVectorTraits::SetLength( lower, measurementVectorSize );
  MeasurementVectorTraits::SetLength( upper, measurementVectorSize );

  typename HistogramType::MeasurementVectorType h_upper;
  typename HistogramType::MeasurementVectorType h_lower;

  MeasurementVectorTraits::SetLength( h_lower, measurementVectorSize );
  MeasurementVectorTraits::SetLength( h_upper, measurementVectorSize );

  if( autoMinimumMaximum && autoMinimumMaximum->Get() )
    {
    if( inputSample->Size() )
      {
      Algorithm::FindSampleBound( 
        inputSample,  inputSample->Begin(), inputSample->End(), lower, upper);
      
      float margin;

      for( unsigned int i = 0; i < measurementVectorSize; i++ )
        {
        if( !NumericTraits< HistogramMeasurementType >::is_integer )
          {
          margin = 
              ( (HistogramMeasurementType )(upper[i] - lower[i]) / 
                (HistogramMeasurementType ) histogramSize[i] ) / 
              (HistogramMeasurementType ) marginalScale;
          h_upper[i] = (HistogramMeasurementType ) (upper[i] + margin);
          if(h_upper[i] <= upper[i])
            { 
            // an overflow has occurred therefore set upper to upper
            h_upper[i] = upper[i];
            // Histogram measurement type would force the clipping the max value.
            // Therefore we must call the following to include the max value:
            outputHistogram->SetClipBinsAtEnds(false);
            // The above function is okay since here we are within the autoMinMax 
            // computation and clearly the user intended to include min and max.
            }
          }
        else
          {
          h_upper[i] = ((HistogramMeasurementType ) upper[i]) + 
            NumericTraits< HistogramMeasurementType  >::One;
          if(h_upper[i] <= upper[i])
            { 
            // an overflow has occurred therefore set upper to upper
            h_upper[i] = upper[i];
            // Histogram measurement type would force the clipping the max value.
            // Therefore we must call the following to include the max value:
            outputHistogram->SetClipBinsAtEnds(false);
            // The above function is okay since here we are within the autoMinMax 
            // computation and clearly the user intended to include min and max.
            }
          }
        h_lower[i] = ( HistogramMeasurementType ) lower[i];
        }
      }
    else
      {
      for( unsigned int i = 0; i < measurementVectorSize; i++ )
        {
        h_lower[i] = ( HistogramMeasurementType ) lower[i];
        h_upper[i] = ( HistogramMeasurementType ) upper[i];
        }
      }
    }
  else
    {
    if( binMaximumObject == NULL )
      {
      itkExceptionMacro("Histogram Bin Maximum input is missing");
      }

    if( binMinimumObject == NULL )
      {
      itkExceptionMacro("Histogram Bin Minimum input is missing");
      }

    h_upper = binMaximumObject->Get();
    h_lower = binMinimumObject->Get();
    }

  // initialize the Histogram object using the sizes and
  // the upper and lower bound from the FindSampleBound function
  outputHistogram->Initialize( histogramSize, h_lower, h_upper );

  typename SampleType::ConstIterator iter = inputSample->Begin();
  typename SampleType::ConstIterator last = inputSample->End();
  typename HistogramType::IndexType index;
  typename SampleType::MeasurementVectorType lvector;
  typename HistogramType::MeasurementVectorType hvector;
  unsigned int i;
  while (iter != last)
    {
    lvector = iter.GetMeasurementVector();
    for ( i = 0; i < HistogramType::MeasurementVectorSize; i++)
      {
      hvector[i] = (HistogramMeasurementType) lvector[i];
      }

    outputHistogram->GetIndex( hvector, index );
    if( !outputHistogram->IsIndexOutOfBounds( index ) )
      {
      // if the measurement vector is out of bound then
      // the GetIndex method has returned an index set to the max size of
      // the invalid dimension - even if the hvector is less than the minimum
      // bin value.
      // If the index isn't valid, we don't increase the frequency.
      // See the comments in Histogram->GetIndex() for more info.
      outputHistogram->IncreaseFrequency( index, 1 );
      }
    ++iter;
    }

}

} // end of namespace Statistics 
} // end of namespace itk

#endif
