/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCovarianceFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/07/26 15:54:54 $
  Version:   $Revision: 1.18 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkCovarianceFilter_txx
#define __itkCovarianceFilter_txx

#include "itkMeasurementVectorTraits.h"

namespace itk { 
namespace Statistics {

template< class TSample >
CovarianceFilter< TSample >
::CovarianceFilter()  
{
  this->ProcessObject::SetNumberOfRequiredInputs(1);
  this->ProcessObject::SetNumberOfRequiredOutputs(2);

  typename MatrixDecoratedType::Pointer matrixDecorator = 
    static_cast< MatrixDecoratedType * >( this->MakeOutput(0).GetPointer() );
  this->ProcessObject::SetNthOutput(0, matrixDecorator.GetPointer());
  
  typename MeasurementVectorDecoratedType::Pointer measurementVectorDecorator = 
    static_cast< MeasurementVectorDecoratedType * >( this->MakeOutput(1).GetPointer() );
  this->ProcessObject::SetNthOutput(1, measurementVectorDecorator.GetPointer());
}

template< class TSample >
CovarianceFilter< TSample >
::~CovarianceFilter()  
{
}

template< class TSample >
void
CovarianceFilter< TSample >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

template< class TSample >
void
CovarianceFilter< TSample >
::SetInput( const SampleType * sample )
{
  this->ProcessObject::SetNthInput(0, 
                                   const_cast< SampleType* >( sample ) );
}

template< class TSample >
const TSample *
CovarianceFilter< TSample >
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
typename CovarianceFilter< TSample>::DataObjectPointer
CovarianceFilter< TSample >
::MakeOutput(unsigned int index )
{
  if ( index == 0 )
    {
    return static_cast< DataObject * >(MatrixDecoratedType::New().GetPointer());
    }

  if ( index == 1 )
    {
    return static_cast< DataObject * >(MeasurementVectorDecoratedType::New().GetPointer());
    }
  itkExceptionMacro("Trying to create output of index " << index << " larger than the number of output");
}

template< class TSample >
inline void
CovarianceFilter< TSample >
::GenerateData() 
{
  const SampleType *input = this->GetInput();

  MeasurementVectorSizeType measurementVectorSize = 
                             input->GetMeasurementVectorSize();
 
  MatrixDecoratedType * decoratedOutput =
            static_cast< MatrixDecoratedType * >(
              this->ProcessObject::GetOutput(0));

  MatrixType output = decoratedOutput->Get();

  MeasurementVectorDecoratedType * decoratedMeanOutput =
            static_cast< MeasurementVectorDecoratedType * >(
              this->ProcessObject::GetOutput(1));

  output.SetSize( measurementVectorSize, measurementVectorSize );
  output.Fill(0.0);

  MeasurementVectorType mean;

  MeasurementVectorTraits::SetLength( mean, measurementVectorSize );

  mean.Fill(0.0);

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
      mean[i] += frequency * measurements[i];
      }
    ++iter;
    }

  for( unsigned int i = 0; i < measurementVectorSize; ++i )
    {
    mean[i] = mean[i] / totalFrequency;
    }

   decoratedMeanOutput->Set( mean );

  //reset the total frequency and iterator
  iter = input->Begin();
  // fills the lower triangle and the diagonal cells in the covariance matrix
  while (iter != end)
    {
    frequency = iter.GetFrequency();
    measurements = iter.GetMeasurementVector();
    for ( unsigned int i = 0; i < measurementVectorSize; ++i )
      {
      diff[i] = measurements[i] - mean[i];
      }

    // updates the covariance matrix
    for( unsigned int row = 0; row < measurementVectorSize; row++ )
      {
      for( unsigned int col = 0; col < row + 1; col++)
        {
        output(row,col) += frequency * diff[row] * diff[col];
        }
      }
    ++iter;
    }

  // fills the upper triangle using the lower triangle  
  for( unsigned int row = 1; row < measurementVectorSize; row++)
    {
    for( unsigned int col = 0; col < row; col++)
      {
      output(col, row) = output(row, col);
      } 
    }

  output /= ( totalFrequency - 1.0 );

  decoratedOutput->Set( output );
}

template< class TSample >
const typename CovarianceFilter< TSample>::MatrixDecoratedType *
CovarianceFilter< TSample >
::GetCovarianceMatrixOutput() const
{
  return static_cast<const MatrixDecoratedType *>(this->ProcessObject::GetOutput(0));
}

template< class TSample >
const typename CovarianceFilter< TSample>::MatrixType
CovarianceFilter< TSample >
::GetCovarianceMatrix() const 
{
  return this->GetCovarianceMatrixOutput()->Get();
}

template< class TSample >
const typename CovarianceFilter< TSample>::MeasurementVectorDecoratedType *
CovarianceFilter< TSample >
::GetMeanOutput() const
{
  return static_cast<const MeasurementVectorDecoratedType *>(this->ProcessObject::GetOutput(1));
}

template< class TSample >
const typename CovarianceFilter< TSample>::MeasurementVectorType
CovarianceFilter< TSample >
::GetMean() const 
{
  return this->GetMeanOutput()->Get();
}

} // end of namespace Statistics 
} // end of namespace itk

#endif
