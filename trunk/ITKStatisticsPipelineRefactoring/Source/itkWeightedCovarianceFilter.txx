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
  this->ProcessObject::SetNumberOfRequiredInputs(1);
  this->ProcessObject::SetNumberOfRequiredOutputs(1);

  typename MatrixDecoratedType::Pointer matrixDecorator = 
    static_cast< MatrixDecoratedType * >( this->MakeOutput(0).GetPointer() );
  this->ProcessObject::SetNthOutput(0, matrixDecorator.GetPointer());
  
  typename MeasurementVectorDecoratedType::Pointer measurementVectorDecorator = 
    static_cast< MeasurementVectorDecoratedType * >( this->MakeOutput(1).GetPointer() );
  this->ProcessObject::SetNthOutput(1, measurementVectorDecorator.GetPointer());

  // initialize parameters
  m_WeightFunction = NULL;
  m_Weights        = 0;
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
::SetInput( const SampleType * sample )
{
  this->ProcessObject::SetNthInput(0, 
                                   const_cast< SampleType* >( sample ) );
  //initialize the weight array
  m_Weights.SetSize( sample->Size() );
  m_Weights.Fill(1.0); // set equal weights
}

template< class TSample >
void
WeightedCovarianceFilter< TSample >
::SetWeights(WeightArrayType array)
{
  m_Weights = array;
  this->Modified();
}

template< class TSample >
typename WeightedCovarianceFilter< TSample >::WeightArrayType
WeightedCovarianceFilter< TSample >
::GetWeights()
{
  return m_Weights;
}

template< class TSample >
void
WeightedCovarianceFilter< TSample >
::SetWeightFunction(WeightFunctionType* func)
{
  m_WeightFunction = func;
  this->Modified();
}

template< class TSample >
typename WeightedCovarianceFilter< TSample >::WeightFunctionType*
WeightedCovarianceFilter< TSample >
::GetWeightFunction()
{
  return m_WeightFunction;
}

template< class TSample >
const TSample *
WeightedCovarianceFilter< TSample >
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
typename WeightedCovarianceFilter< TSample>::DataObjectPointer
WeightedCovarianceFilter< TSample >
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

  double frequency;
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

  double frequency;
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

template< class TSample >
const typename WeightedCovarianceFilter< TSample>::MatrixDecoratedType *
WeightedCovarianceFilter< TSample >
::GetWeightedCovarianceMatrixOutput() const
{
  return static_cast<const MatrixDecoratedType *>(this->ProcessObject::GetOutput(0));
}

template< class TSample >
const typename WeightedCovarianceFilter< TSample>::MatrixType
WeightedCovarianceFilter< TSample >
::GetWeightedCovarianceMatrix() const 
{
  return this->GetWeightedCovarianceMatrixOutput()->Get();
}

template< class TSample >
const typename WeightedCovarianceFilter< TSample>::MeasurementVectorDecoratedType *
WeightedCovarianceFilter< TSample >
::GetMeanOutput() const
{
  return static_cast<const MeasurementVectorDecoratedType *>(this->ProcessObject::GetOutput(1));
}

template< class TSample >
const typename WeightedCovarianceFilter< TSample>::MeasurementVectorType
WeightedCovarianceFilter< TSample >
::GetMean() const 
{
  return this->GetMeanOutput()->Get();
}

} // end of namespace Statistics 
} // end of namespace itk

#endif
