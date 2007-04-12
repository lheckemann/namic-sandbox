/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkWeightedMeanFilter.txx,v $
  Language:  C++
  Date:      $Date: 2006/08/14 15:40:56 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkWeightedMeanFilter_txx
#define __itkWeightedMeanFilter_txx

#include "itkMeasurementVectorTraits.h"
#include "itkNumericTraits.h"

namespace itk { 
namespace Statistics {

template< class TSample >
WeightedMeanFilter< TSample >
::WeightedMeanFilter()
{
  this->ProcessObject::SetNumberOfRequiredInputs(1);
  this->ProcessObject::SetNumberOfRequiredOutputs(1);

  typename MeasurementVectorDecoratedType::Pointer measurementVectorDecorator = 
    static_cast< MeasurementVectorDecoratedType * >( this->MakeOutput(0).GetPointer() );
  this->ProcessObject::SetNthOutput(0, measurementVectorDecorator.GetPointer());

  m_WeightFunction = 0;
}

template< class TSample >
void
WeightedMeanFilter< TSample >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

template< class TSample >
void
WeightedMeanFilter< TSample >
::SetInput( const SampleType * sample )
{
  this->ProcessObject::SetNthInput(0, 
                                   const_cast< SampleType* >( sample ) );

  //initialize the weight array 
  m_Weights.SetSize( sample->Size() );
  m_Weights.Fill(1.0); //set equal weights
}

template< class TSample >
const TSample *
WeightedMeanFilter< TSample >
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
const typename WeightedMeanFilter< TSample >::WeightFunctionType *
WeightedMeanFilter< TSample >
::GetWeightFunction()
{
  return m_WeightFunction;
}


template< class TSample >
typename WeightedMeanFilter< TSample>::DataObjectPointer
WeightedMeanFilter< TSample >
::MakeOutput(unsigned int itkNotUsed(idx))
{
  return static_cast< DataObject * >(MeasurementVectorDecoratedType::New().GetPointer());
}

template< class TSample >
const typename WeightedMeanFilter< TSample>::MeasurementVectorDecoratedType *
WeightedMeanFilter< TSample >
::GetOutput() const
{
  return static_cast< const MeasurementVectorDecoratedType * >(
              this->ProcessObject::GetOutput(0));
}

template< class TSample >
void
WeightedMeanFilter< TSample >
::GenerateData() 
{
  const SampleType *input = this->GetInput();

  MeasurementVectorSizeType measurementVectorSize = 
                             input->GetMeasurementVectorSize();
 
  MeasurementVectorDecoratedType * decoratedOutput =
            static_cast< MeasurementVectorDecoratedType * >(
              this->ProcessObject::GetOutput(0));

  MeasurementVectorType output = decoratedOutput->Get();

  //reset the output
  for (unsigned int dim = 0; dim < measurementVectorSize; dim++)
    {
    output[dim] = NumericTraits< MeasurementType >::Zero;
    }
  
  typename TSample::ConstIterator iter = input->Begin();
  typename TSample::ConstIterator end =  input->End();
  double totalWeight = 0.0;
  double weight;
  int measurementVectorIndex = 0;
  typename TSample::MeasurementVectorType measurements;

  if (m_WeightFunction != 0) 
    {
    while (iter != end)
      {
      measurements = iter.GetMeasurementVector();
      weight = 
        iter.GetFrequency() * m_WeightFunction->Evaluate(measurements);
      totalWeight += weight;
      for ( unsigned int dim = 0; dim < measurementVectorSize; dim++ )
        {
        output[dim] += measurements[dim] * weight;
        }
      ++iter;
      }
    }
  else
    {
    while (iter != end)
      {
      measurements = iter.GetMeasurementVector();
      weight = iter.GetFrequency() * (m_Weights)[measurementVectorIndex];
      totalWeight += weight;

      for ( unsigned int dim = 0; dim < measurementVectorSize; dim++ )
        {
        output[dim] += measurements[dim] * weight;
        }
      ++measurementVectorIndex;
      ++iter;
      }
    }

  for (unsigned int dim = 0; dim < measurementVectorSize; dim++)
    {
    output[dim] /= totalWeight;
    }

  decoratedOutput->Set( output );
 
}

} // end of namespace Statistics 
} // end of namespace itk

#endif
