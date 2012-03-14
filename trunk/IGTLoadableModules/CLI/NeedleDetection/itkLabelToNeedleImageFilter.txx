/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLabelToNeedleImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2008-10-16 16:45:10 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLabelToNeedleImageFilter_txx
#define __itkLabelToNeedleImageFilter_txx

#include "itkLabelToNeedleImageFilter.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

#include "vnl/vnl_math.h"

namespace itk
{

/**
 * Constructor
 */
template < typename  TInput, typename TOutput  >
LabelToNeedleImageFilter< TInput, TOutput >
::LabelToNeedleImageFilter()
{
}


template < typename  TInput, typename TOutput  >
void 
LabelToNeedleImageFilter< TInput, TOutput >
::GenerateData()
{
  itkDebugMacro(<< "LabelToNeedleImageFilter generating data ");
  
  //typedef LabelStatisticsImageFilter< InputImageType, InputImageType > LabelStatisticsImageFilterType;
  //typename LabelStatisticsImageFilterType::Pointer labelStatisticsImageFilter = LabelStatisticsImageFilterType::New();
  //
  //labelStatisticsImageFilter->SetLabelInput( this->GetInput() );
  //labelStatisticsImageFilter->SetInput( this->GetInput() );
  //labelStatisticsImageFilter->Update();
  //
  //std::cout << "Number of labels: " << labelStatisticsImageFilter->GetNumberOfLabels() << std::endl;
  //std::cout << std::endl;  

  typename InputImageType::ConstPointer input = this->GetInput();
  typename OutputImageType::Pointer output = this->GetOutput();

  ImageRegionConstIterator<InputImageType> it;
  it = ImageRegionConstIterator<InputImageType>( input, input->GetRequestedRegion() );
  ImageRegionIterator<OutputImageType> oit;
  this->AllocateOutputs();
  oit = ImageRegionIterator<OutputImageType>(output,
                                             output->GetRequestedRegion());
  
  oit.GoToBegin();
  it.GoToBegin();
  
  typedef InputImageType::IndexType IndexType;
  typedef std::list< IndexType > IndexListType;
  typedef std::map< InputPixelType, IndexListType > IndexListMapType;
  typedef std::map< InputPixelType, IndexType > SumMapType;

  typename IndexListMapType map;
  typename SumMapType sum;
  typedef vnl_matrix< double > MatrixType;

  while (!it.IsAtEnd())
    {
    InputPixelType pix = it.Get();
    IndexType index = it.GetIndex();
    map[pix].push_back(index);
    SumMapType::iterator it = sum.find(pix);
    if (it == sum.end())
      {
      sum[pix] = IndexType(0,0,0);
      }
    IndexType s = sum[pix];
    s[0] += index[0];
    s[1] += index[1];
    s[2] += index[2];
    //oit.Set(it.Get());
    ++it;
    ++oit;
    }
  int nLabel = map.size();

  IndexListMapType::iterator miter;
  for (miter = map.begin(); miter != map.end(); miter ++)
    {
    InputPixelType pix = miter->first;
    IndexListType il = miter->second; 
    IndexType s = sum[pix];

    // Calculate average
    int       n = il.size();
    double   ax = (double)s[0] / (double)n;
    double   ay = (double)s[1] / (double)n;
    double   az = (double)s[2] / (double)n;

    MatrixType B(3, n);
    int i = 0;
    IndexListType::iterator iter;
    for (iter = il.begin(); iter != il.end(); il ++)
      {
      r[0][i] = *iter[0] - ax;
      r[1][i] = *iter[1] - ay;
      r[2][i] = *iter[2] - az;
      }

    typedef itk::SymmetricEigenAnalysis< InputMatrixType,
      EigenValuesArrayType, EigenVectorMatrixType > SymmetricEigenAnalysisType;

    typedef  itk::Statistics::CovarianceCalculator< double >   CovarianceAlgorithmType;
    typename CovarianceAlgorithmType::Pointer covarianceAlgorithm = 
      CovarianceAlgorithmType::New();

    typedef itk::Statistics::MeanCalculator< SampleType > MeanAlgorithmType;
    covarianceAlgorithm->SetInput( m_Sample );

    MatrixType C = B * B.conjugate_transpose();

    InputMatrixType Cvnl(C);

    SymmetricEigenAnalysis analysis;
    OutputImagePixelType eigenValue;
    EigenMatrixType eigenMatrix;
    typedef Matrix< double, 3, 3 >  EigenMatrixType;

    analysis.ComputeEigenValuesAndVectors(Cvnl, eigenValue, eigenMatrix );    


    }
  
  
  

}


template < typename  TInput, typename TOutput  >
void
LabelToNeedleImageFilter< TInput, TOutput >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace itk
  
#endif
