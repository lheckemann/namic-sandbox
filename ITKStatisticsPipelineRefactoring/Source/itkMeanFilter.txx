/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMeanFilter.txx,v $
  Language:  C++
  Date:      $Date: 2006/08/14 15:40:56 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMeanFilter_txx
#define __itkMeanFilter_txx

#include "itkMeasurementVectorTraits.h"

namespace itk { 
namespace Statistics {

template< class TSample >
MeanFilter< TSample >
::MeanFilter()
{
  this->ProcessObject::SetNumberOfRequiredInputs(1);
  this->ProcessObject::SetNumberOfRequiredOutputs(1);

  typename MeasurementVectorDecoratedType::Pointer measurementVectorDecorator = 
    static_cast< MeasurementVectorDecoratedType * >( this->MakeOutput(0).GetPointer() );
  this->ProcessObject::SetNthOutput(0, measurementVectorDecorator.GetPointer());
}


template< class TSample >
MeanFilter< TSample >
::~MeanFilter()
{
}


template< class TSample >
void
MeanFilter< TSample >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

template< class TSample >
void
MeanFilter< TSample >
::SetInput( const SampleType * sample )
{
  this->ProcessObject::SetNthInput(0, 
                                   const_cast< SampleType* >( sample ) );
}

template< class TSample >
const TSample *
MeanFilter< TSample >
::GetInput( ) const
{
  if (this->GetNumberOfInputs() < 1)
    {
    return 0;
    }

  return static_cast<const SampleType * >
  (this->ProcessObject::GetInput(0) );
}

template< class TSample >
typename MeanFilter< TSample>::DataObjectPointer
MeanFilter< TSample >
::MakeOutput(unsigned int itkNotUsed(idx))
{
  return static_cast< DataObject * >(MeasurementVectorDecoratedType::New().GetPointer());
}

template< class TSample >
const typename MeanFilter< TSample>::MeasurementVectorDecoratedType *
MeanFilter< TSample >
::GetOutput() const
{
  return static_cast< const MeasurementVectorDecoratedType * >(
              this->ProcessObject::GetOutput(0));
}

template< class TSample >
const typename MeanFilter< TSample>::MeasurementVectorType 
MeanFilter< TSample >
::GetMean() const
{
  return this->GetOutput()->Get(); 
}

template< class TSample >
void
MeanFilter< TSample >
::GenerateData() 
{
  const SampleType *input = this->GetInput();

  MeasurementVectorSizeType measurementVectorSize = 
                             input->GetMeasurementVectorSize();
 
  MeasurementVectorDecoratedType * decoratedOutput =
            static_cast< MeasurementVectorDecoratedType * >(
              this->ProcessObject::GetOutput(0));

  MeasurementVectorType output = decoratedOutput->Get();

  typename TSample::ConstIterator iter = input->Begin();
  typename TSample::ConstIterator end =  input->End();
  double totalFrequency = 0.0;

  for (unsigned int dim = 0; dim < measurementVectorSize; dim++)
    {
    output[dim] = itk::NumericTraits< MeasurementType >::Zero;
    }

  while (iter != end)
    {
    double frequency = iter.GetFrequency();
    totalFrequency += frequency;

    for (unsigned int dim = 0; dim < measurementVectorSize; dim++)
      {
      output[dim] += iter.GetMeasurementVector()[dim] * frequency;
      }
    ++iter;
    }

  // compute the mean if the total frequency is different from zero 
  if ( totalFrequency != 0.0 )
    {
    for (unsigned int dim = 0; dim < measurementVectorSize; dim++)
      {
      output[dim] /= totalFrequency;
      }
    }

  decoratedOutput->Set( output );
 
}

} // end of namespace Statistics 
} // end of namespace itk

#endif
