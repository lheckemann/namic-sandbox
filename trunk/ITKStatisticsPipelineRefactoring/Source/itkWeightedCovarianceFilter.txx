/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkWeightedCovarianceFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/07/26 15:54:54 $
  Version:   $Revision: 1.18 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkWeightedCovarianceFilter_txx
#define __itkWeightedCovarianceFilter_txx

namespace itk { 
namespace Statistics {

template< class TSample >
WeightedCovarianceFilter< TSample >
::WeightedCovarianceFilter()  
{
  // initialize parameters
  m_WeightFunction = NULL;
  m_Weights        = 0;
}


template< class TSample >
WeightedCovarianceFilter< TSample >
::~WeightedCovarianceFilter()  
{
}


template< class TSample >
void
WeightedCovarianceFilter< TSample >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}


template< class TSample >
void
WeightedCovarianceFilter< TSample >
::SetWeights(const WeightArrayType & array)
{
  m_Weights = array;
  this->Modified();
}

template< class TSample >
const typename WeightedCovarianceFilter< TSample >::WeightArrayType &
WeightedCovarianceFilter< TSample >
::GetWeights() const
{
  return m_Weights;
}

template< class TSample >
void
WeightedCovarianceFilter< TSample >
::SetWeightFunction(const WeightFunctionType* func)
{
  // FIXME: it should have checked if it was the same value
  m_WeightFunction = func;
  this->Modified();
}

template< class TSample >
const typename WeightedCovarianceFilter< TSample >::WeightFunctionType*
WeightedCovarianceFilter< TSample >
::GetWeightFunction() const
{
  return m_WeightFunction;
}


template< class TSample >
inline void
WeightedCovarianceFilter< TSample >
::GenerateData() 
{
  // if weighting function is specifed, use it to compute the covariance matrix 
  if ( m_WeightFunction != NULL ) 
    {
    this->ComputeCovarianceMatrixWithWeightingFunction(); 
    return;
    }

  this->ComputeCovarianceMatrixWithWeights(); 
} 

template< class TSample >
inline void
WeightedCovarianceFilter< TSample >
::ComputeCovarianceMatrixWithWeightingFunction() 
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
  mean.Fill(0.0);

  double totalFrequency = 0.0;

  typename TSample::ConstIterator iter = input->Begin();
  typename TSample::ConstIterator end = input->End();

  MeasurementVectorType diff;
  MeasurementVectorType measurements;

  double weight;
  double totalWeight = 0.0;
  double sumSquaredWeight=0.0;

  //Compute the mean first

  unsigned int measurementVectorIndex = 0;
  while (iter != end)
    {
    measurements = iter.GetMeasurementVector();
    weight = iter.GetFrequency() * m_WeightFunction->Evaluate(measurements);
    totalWeight += weight;
    sumSquaredWeight += weight * weight;

    for( unsigned int i = 0; i < measurementVectorSize; ++i )
      {
      mean[i] += weight * measurements[i];
      }
    ++iter;
    }

  for( unsigned int i = 0; i < measurementVectorSize; ++i )
    {
    mean[i] = mean[i] / totalWeight;
    }

   decoratedMeanOutput->Set( mean );

  //reset iterator
  iter = input->Begin();
  // fills the lower triangle and the diagonal cells in the covariance matrix
  while (iter != end)
    {
    measurements = iter.GetMeasurementVector();
    weight = iter.GetFrequency() * m_WeightFunction->Evaluate(measurements);
    for ( unsigned int i = 0; i < measurementVectorSize; ++i )
      {
      diff[i] = measurements[i] - mean[i];
      }

    // updates the covariance matrix
    for( unsigned int row = 0; row < measurementVectorSize; row++ )
      {
      for( unsigned int col = 0; col < row + 1; col++)
        {
        output(row,col) += weight * diff[row] * diff[col];
        }
      }
    ++iter;
    ++measurementVectorIndex;
    }

  // fills the upper triangle using the lower triangle  
  for( unsigned int row = 1; row < measurementVectorSize; row++)
    {
    for( unsigned int col = 0; col < row; col++)
      {
      output(col, row) = output(row, col);
      } 
    }

  output /= ( totalWeight - ( sumSquaredWeight / totalWeight ) );

  decoratedOutput->Set( output );
}


template< class TSample >
inline void
WeightedCovarianceFilter< TSample >
::ComputeCovarianceMatrixWithWeights() 
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
  mean.Fill(0.0);

  double totalFrequency = 0.0;

  typename TSample::ConstIterator iter = input->Begin();
  typename TSample::ConstIterator end = input->End();

  MeasurementVectorType diff;
  MeasurementVectorType measurements;

  double weight;
  double totalWeight = 0.0;
  double sumSquaredWeight=0.0;

  if( m_Weights.Size() != input->Size() )
    {
    itkExceptionMacro("Size of weights array doesn't match the sample size");
    }

  //Compute the mean first

  unsigned int measurementVectorIndex = 0;
  while (iter != end)
    {
    measurements = iter.GetMeasurementVector();
    weight = iter.GetFrequency() * (m_Weights)[measurementVectorIndex];
    totalWeight += weight;
    sumSquaredWeight += weight * weight;

    for( unsigned int i = 0; i < measurementVectorSize; ++i )
      {
      mean[i] += weight * measurements[i];
      }
    ++iter;
    }

  for( unsigned int i = 0; i < measurementVectorSize; ++i )
    {
    mean[i] = mean[i] / totalWeight;
    }

   decoratedMeanOutput->Set( mean );

  //reset iterator
  iter = input->Begin();
  // fills the lower triangle and the diagonal cells in the covariance matrix
  while (iter != end)
    {
    weight = iter.GetFrequency() * (m_Weights)[measurementVectorIndex];
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
        output(row,col) += weight * diff[row] * diff[col];
        }
      }
    ++iter;
    ++measurementVectorIndex;
    }

  // fills the upper triangle using the lower triangle  
  for( unsigned int row = 1; row < measurementVectorSize; row++)
    {
    for( unsigned int col = 0; col < row; col++)
      {
      output(col, row) = output(row, col);
      } 
    }

  output /= ( totalWeight - ( sumSquaredWeight / totalWeight ) );

  decoratedOutput->Set( output );
}

} // end of namespace Statistics 
} // end of namespace itk

#endif
