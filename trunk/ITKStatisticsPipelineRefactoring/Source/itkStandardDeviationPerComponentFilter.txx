/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkStandardDeviationPerComponentFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/07/26 15:54:54 $
  Version:   $Revision: 1.18 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkStandardDeviationPerComponentFilter_txx
#define __itkStandardDeviationPerComponentFilter_txx

#include "itkMeasurementVectorTraits.h"
#include "vnl/vnl_math.h"

namespace itk { 
namespace Statistics {

template< class TSample >
StandardDeviationPerComponentFilter< TSample >
::StandardDeviationPerComponentFilter()  
{
  this->ProcessObject::SetNumberOfRequiredInputs(1);
  this->ProcessObject::SetNumberOfRequiredOutputs(1);

  typename MeasurementVectorRealDecoratedType::Pointer measurementVectorDecorator = 
    static_cast< MeasurementVectorRealDecoratedType * >( this->MakeOutput(0).GetPointer() );
  this->ProcessObject::SetNthOutput(1, measurementVectorDecorator.GetPointer());
}

template< class TSample >
StandardDeviationPerComponentFilter< TSample >
::~StandardDeviationPerComponentFilter()  
{
}

template< class TSample >
void
StandardDeviationPerComponentFilter< TSample >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

template< class TSample >
void
StandardDeviationPerComponentFilter< TSample >
::SetInput( const SampleType * sample )
{
  this->ProcessObject::SetNthInput(0, const_cast< SampleType* >( sample ) );
}

template< class TSample >
const TSample *
StandardDeviationPerComponentFilter< TSample >
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
typename StandardDeviationPerComponentFilter< TSample>::DataObjectPointer
StandardDeviationPerComponentFilter< TSample >
::MakeOutput(unsigned int index )
{
  if ( index == 0 )
    {
    typedef typename MeasurementVectorTraitsTypes< MeasurementVectorType >::ValueType   ValueType;
    MeasurementVectorType mean;
    MeasurementVectorTraits::SetLength( mean,  this->GetMeasurementVectorSize() );
    mean.Fill( NumericTraits< ValueType >::Zero );
    typename MeasurementVectorRealDecoratedType::Pointer decoratedStandardDeviation = MeasurementVectorRealDecoratedType::New();
    decoratedStandardDeviation->Set( mean ); 
    return static_cast< DataObject * >( decoratedStandardDeviation.GetPointer() );
    }
  itkExceptionMacro("Trying to create output of index " << index << " larger than the number of output");
}

template< class TSample >
typename StandardDeviationPerComponentFilter< TSample >::MeasurementVectorSizeType 
StandardDeviationPerComponentFilter< TSample >
::GetMeasurementVectorSize() const
{
  const SampleType *input = this->GetInput();

  if( input )
    {
    return input->GetMeasurementVectorSize();
    }

  // Test if the Vector type knows its length
  MeasurementVectorType vector;
  MeasurementVectorSizeType measurementVectorSize = MeasurementVectorTraits::GetLength( vector );

  if( measurementVectorSize )
    {
    return measurementVectorSize;
    }

  measurementVectorSize = 1; // Otherwise set it to an innocuous value

  return measurementVectorSize;
}
 

template< class TSample >
inline void
StandardDeviationPerComponentFilter< TSample >
::GenerateData() 
{
  const SampleType *input = this->GetInput();

  MeasurementVectorSizeType measurementVectorSize = input->GetMeasurementVectorSize();
 
  MeasurementVectorRealDecoratedType * decoratedStandardDeviationOutput =
            static_cast< MeasurementVectorRealDecoratedType * >(
              this->ProcessObject::GetOutput(0));

  MeasurementVectorRealType sum;
  MeasurementVectorRealType sumOfSquares;
  MeasurementVectorRealType mean;
  MeasurementVectorRealType standardDeviation;

  MeasurementVectorTraits::SetLength( sum, measurementVectorSize );
  MeasurementVectorTraits::SetLength( mean, measurementVectorSize );
  MeasurementVectorTraits::SetLength( sumOfSquares, measurementVectorSize );
  MeasurementVectorTraits::SetLength( standardDeviation, measurementVectorSize );

  sum.Fill(0.0);
  mean.Fill(0.0);
  sumOfSquares.Fill(0.0);
  standardDeviation.Fill(0.0);

  double frequency;
  double totalFrequency = 0.0;

  typename TSample::ConstIterator iter = input->Begin();
  typename TSample::ConstIterator end = input->End();

  MeasurementVectorType diff;
  MeasurementVectorType measurements;

  MeasurementVectorTraits::SetLength( diff, measurementVectorSize );
  MeasurementVectorTraits::SetLength( measurements, measurementVectorSize );

  //Compute the mean first
  while (iter != end)
    {
    frequency = iter.GetFrequency();
    totalFrequency += frequency;
    measurements = iter.GetMeasurementVector();

    for( unsigned int i = 0; i < measurementVectorSize; ++i )
      {
      double value = measurements[i]; 
      sum[i] += frequency * value;
      sumOfSquares[i] += frequency * value * value;
      }
    ++iter;
    }

  for( unsigned int i = 0; i < measurementVectorSize; ++i )
    {
    const double meanValue = sum[i] / totalFrequency;
    mean[i] = meanValue;
    const double variance = 
     ( sumOfSquares[i] - meanValue * meanValue * totalFrequency ) / ( totalFrequency - 1.0 );
    standardDeviation[i] = vcl_sqrt( variance );
    }

  decoratedStandardDeviationOutput->Set( standardDeviation );
}

template< class TSample >
const typename StandardDeviationPerComponentFilter< TSample>::MeasurementVectorRealDecoratedType *
StandardDeviationPerComponentFilter< TSample >
::GetStandardDeviationPerComponentOutput() const
{
  return static_cast<const MeasurementVectorRealDecoratedType *>(this->ProcessObject::GetOutput(0));
}

template< class TSample >
const typename StandardDeviationPerComponentFilter< TSample>::MeasurementVectorRealType
StandardDeviationPerComponentFilter< TSample >
::GetStandardDeviationPerComponent() const 
{
  return this->GetStandardDeviationPerComponentOutput()->Get();
}

} // end of namespace Statistics 
} // end of namespace itk

#endif
