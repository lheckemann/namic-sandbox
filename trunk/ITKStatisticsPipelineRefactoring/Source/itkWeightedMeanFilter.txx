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
}


template< class TSample >
WeightedMeanFilter< TSample >
::~WeightedMeanFilter()
{
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
::GenerateData() 
{
  // if weighting function is specifed, use it to compute the mean
  const  InputWeightingFunctionObjectType * functionObject = 
                                         this->GetWeightingFunctionInput();


  if ( functionObject != NULL ) 
    {
    this->ComputeMeanWithWeightingFunction(); 
    return;
    }

  // if weight array is specified use it to compute the mean
  const  InputWeightArrayObjectType * weightArrayObject = 
                                         this->GetWeightsInput();

  if ( weightArrayObject != NULL )
    {
    this->ComputeMeanWithWeights(); 
    return;
    }
 
  // Otherwise compute the regular mean ( without weight coefficients) 
  Superclass::GenerateData();
  return;
} 

template< class TSample >
void
WeightedMeanFilter< TSample >
::ComputeMeanWithWeights() 
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

  typename TSample::MeasurementVectorType measurements;

  const  InputWeightArrayObjectType * weightArrayObject = this->GetWeightsInput();
  const  WeightArrayType weightArray = weightArrayObject->Get();

  int measurementVectorIndex = 0;

  while (iter != end)
    {
    measurements = iter.GetMeasurementVector();
    weight = iter.GetFrequency() * (weightArray)[measurementVectorIndex];
    totalWeight += weight;

    for ( unsigned int dim = 0; dim < measurementVectorSize; dim++ )
      {
      output[dim] += measurements[dim] * weight;
      }
    ++measurementVectorIndex;
    ++iter;
    }

  if ( totalWeight != 0.0 )
  {
    for (unsigned int dim = 0; dim < measurementVectorSize; dim++)
    {
    output[dim] /= totalWeight;
    }
  }

  decoratedOutput->Set( output );
 
}

template< class TSample >
void
WeightedMeanFilter< TSample >
::ComputeMeanWithWeightingFunction() 
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

  typename TSample::MeasurementVectorType measurements;

  // if weighting function is specifed, use it to compute the mean
  const  InputWeightingFunctionObjectType * functionObject = 
                                         this->GetWeightingFunctionInput();

  const  WeightingFunctionType * weightFunction = functionObject->Get();

  while (iter != end)
    {
    measurements = iter.GetMeasurementVector();
    weight = 
      iter.GetFrequency() * weightFunction->Evaluate(measurements);
    totalWeight += weight;
    for ( unsigned int dim = 0; dim < measurementVectorSize; dim++ )
      {
      output[dim] += measurements[dim] * weight;
      }
    ++iter;
    }

  if ( totalWeight != 0.0 )
  {
    for (unsigned int dim = 0; dim < measurementVectorSize; dim++)
    {
    output[dim] /= totalWeight;
    }
  }

  decoratedOutput->Set( output );
 
}

} // end of namespace Statistics 
} // end of namespace itk

#endif
