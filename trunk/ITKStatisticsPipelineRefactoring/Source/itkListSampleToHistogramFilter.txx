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
  typename InputHistogramSizeObjectType::Pointer histogramSizeObject =
    InputHistogramSizeObjectType::New();

  HistogramSizeType histogramSize;
  histogramSize.Fill(0);
  histogramSizeObject->Set( histogramSize );
  this->ProcessObject::SetNthInput( 1, histogramSizeObject );

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
  if (this->GetNumberOfInputs() < 1)
    {
    return NULL;
    }
  const SampleType * input = 
    static_cast<const SampleType * >(this->ProcessObject::GetInput(0) );

  return input;
}


template < class TSample, class THistogram >
void
ListSampleToHistogramFilter< TSample, THistogram >
::SetHistogramSize( const HistogramSizeType & histogramSize )
{
  // first check to see if anything changed
  const InputHistogramSizeObjectType * oldHistogramSizeObject
    = this->GetHistogramSizeInput();

  if( oldHistogramSizeObject && oldHistogramSizeObject->Get() == histogramSize )
    {
    return;
    }
  
  // Create an object for it
  typename InputHistogramSizeObjectType::Pointer  
    newHistogramSizeObject = InputHistogramSizeObjectType::New();
  newHistogramSizeObject->Set( histogramSize );

  this->SetHistogramSizeInput( newHistogramSizeObject );
}


template < class TSample, class THistogram >
void
ListSampleToHistogramFilter< TSample, THistogram >
::SetHistogramSizeInput( const InputHistogramSizeObjectType * input )
{
  if( input != this->GetHistogramSizeInput() )
    {
    this->ProcessObject::SetNthInput(1,
      const_cast<InputHistogramSizeObjectType*>(input));
    this->Modified();
    }
}


template < class TSample, class THistogram >
const typename
ListSampleToHistogramFilter< TSample, THistogram >::InputHistogramSizeObjectType *
ListSampleToHistogramFilter< TSample, THistogram >
::GetHistogramSizeInput() const
{
  const InputHistogramSizeObjectType * histogramSizeObject =
      static_cast<const InputHistogramSizeObjectType *>( 
        this->ProcessObject::GetInput(1) );
  
  return histogramSizeObject;
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


} // end of namespace StatisticsNew 
} // end of namespace itk

#endif
