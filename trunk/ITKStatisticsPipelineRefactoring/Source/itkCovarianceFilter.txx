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

namespace itk { 
namespace Statistics {

template< class TSample >
CovarianceFilter< TSample >
::CovarianceFilter()  
{
  this->ProcessObject::SetNumberOfRequiredInputs(1);
  this->ProcessObject::SetNumberOfRequiredOutputs(1);

  typename MatrixDecoratedType::Pointer matrixDecorator = 
    static_cast< MatrixDecoratedType * >( this->MakeOutput(0).GetPointer() );
  this->ProcessObject::SetNthOutput(0, matrixDecorator.GetPointer());
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
::MakeOutput(unsigned int itkNotUsed(idx))
{
  return static_cast< DataObject * >(MatrixDecoratedType::New().GetPointer());
}

template< class TSample >
const typename CovarianceFilter< TSample>::MatrixDecoratedType *
CovarianceFilter< TSample >
::GetOutput() const
{
  return static_cast< const MatrixDecoratedType * >(
              this->ProcessObject::GetOutput(0));
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

  std::cout << "Output1: " << output << std::endl;

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

  std::cout << "The mean is: " << mean << std::endl;

  //reset the total frequency and iterator
  totalFrequency = 0.0;
  iter = input->Begin();
  // fills the lower triangle and the diagonal cells in the covariance matrix
  while (iter != end)
    {
    frequency = iter.GetFrequency();
    totalFrequency += frequency;
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

  std::cout << "Output2: " << output << std::endl;

  // fills the upper triangle using the lower triangle  
  for( unsigned int row = 1; row < measurementVectorSize; row++)
    {
    for( unsigned int col = 0; col < row; col++)
      {
      output(col, row) = output(row, col);
      } 
    }

  std::cout << "Output3: " << output << std::endl;

  output /= ( totalFrequency - 1.0 );

  decoratedOutput->Set( output );
}


} // end of namespace Statistics 
} // end of namespace itk

#endif
