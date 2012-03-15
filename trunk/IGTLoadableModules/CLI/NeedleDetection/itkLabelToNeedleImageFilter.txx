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
#include "itkListSample.h"
#include "itkCovarianceSampleFilter.h"
//#include "itkCovarianceCalculator.h"
#include "itkSymmetricEigenAnalysis.h"
#include "itkCrossHelper.h"

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

  output->FillBuffer(static_cast<OutputPixelType>(0));
  
  const unsigned int VectorLength = 3;
  typedef itk::Vector< double, VectorLength > VectorType;
  typedef itk::Statistics::ListSample< VectorType > PointListType;
  typedef std::map< InputPixelType, PointListType::Pointer > PointListMapType;

  //-- Create lists of points for each label
  PointListMapType pointListMap;
  oit.GoToBegin();
  it.GoToBegin();
  while (!it.IsAtEnd())
    {
    InputPixelType pix = it.Get();
    typename InputImageType::IndexType index = it.GetIndex();
    
    typename PointListMapType::iterator iter = pointListMap.find(pix);
    if (iter == pointListMap.end())
      {
      PointListType::Pointer plist = PointListType::New();
      plist->SetMeasurementVectorSize( VectorLength );    
      pointListMap[pix] = plist;
      }

    VectorType mv;
    typename InputImageType::PointType point;
    input->TransformIndexToPhysicalPoint (index, point);
    mv[0] = (double)point[0];
    mv[1] = (double)point[1];
    mv[2] = (double)point[2];
    pointListMap[pix]->PushBack(mv);

    ++it;
    ++oit;
    //oit.Set(it.Get());
    }


  //-- Check the number of labels (clusters)
  //int nLabel = pointListMap.size();
  
  //-- For each label, perform principal component analysis
  double cosThreshold = vcl_cos((m_AngleThreshold / 180.0) * vnl_math::pi);

  VectorType closestPoint;
  closestPoint[0] = m_ClosestPoint[0];
  closestPoint[1] = m_ClosestPoint[1];
  closestPoint[2] = m_ClosestPoint[2];

  double closestDistance = -1;
  VectorType closestPosition;
  VectorType closestNormal;

  typename PointListMapType::iterator miter;
  for (miter = pointListMap.begin(); miter != pointListMap.end(); miter ++)
    {
    InputPixelType pix = miter->first;
    PointListType::Pointer sample = miter->second; 

    std::cout << "=== Label " << pix << "===" << std::endl;
    
    //// Calculate covariance matrix
    //typedef itk::Statistics::MeanCalculator< PointListType > MeanAlgorithmType;
    //MeanAlgorithmType::Pointer meanAlgorithm = MeanAlgorithmType::New();
    //meanAlgorithm->SetInputSample( sample );
    //meanAlgorithm->Update();
    //std::cout << "Sample mean = " << *(meanAlgorithm->GetOutput()) << std::endl;
    
    //typedef itk::Statistics::CovarianceCalculator< PointListType > 
    typedef itk::Statistics::CovarianceSampleFilter< PointListType > 
      CovarianceAlgorithmType;
    CovarianceAlgorithmType::Pointer covarianceAlgorithm = 
      CovarianceAlgorithmType::New();

    //covarianceAlgorithm->SetInputSample( sample );
    //covarianceAlgorithm->SetMean( meanAlgorithm->GetOutput() );
    covarianceAlgorithm->SetInput( sample );
    covarianceAlgorithm->Update();

    std::cout << "Sample covariance = " << std::endl ; 
    //std::cout << *(covarianceAlgorithm->GetOutput()) << std::endl;
    std::cout << covarianceAlgorithm->GetCovarianceMatrix() << std::endl;

    CovarianceAlgorithmType::MeasurementVectorType meanVector;
    meanVector = covarianceAlgorithm->GetMean();
    std::cout << "Sample mean = " << meanVector << std::endl ; 

    // Perform Symmetric Eigen Analysis
    typedef itk::FixedArray< double, VectorLength > EigenValuesArrayType;
    typedef itk::Matrix< double, VectorLength, VectorLength > EigenVectorMatrixType;
    //typedef itk::SymmetricEigenAnalysis< CovarianceAlgorithmType::OutputType,
    typedef itk::SymmetricEigenAnalysis< CovarianceAlgorithmType::MatrixType,
      EigenValuesArrayType, EigenVectorMatrixType > SymmetricEigenAnalysisType;
    SymmetricEigenAnalysisType analysis (VectorLength);
    EigenValuesArrayType eigenValues;
    EigenVectorMatrixType eigenMatrix;
    analysis.SetOrderEigenMagnitudes( true );
    //analysis.ComputeEigenValuesAndVectors( covarianceAlgorithm->GetOutput,
    analysis.ComputeEigenValuesAndVectors( covarianceAlgorithm->GetCovarianceMatrix(),
                                            eigenValues, eigenMatrix );    

    std::cout << "EigenValues: " << eigenValues << std::endl;
    std::cout << "EigenVectors (each row is an an eigen vector): " << std::endl;
    std::cout << eigenMatrix << std::endl;

    // Check the distribution along the eigen vectors
    if ( eigenValues[2] >= m_MinPrincipalAxisLength &&
         sqrt(eigenValues[0]*eigenValues[0]+eigenValues[1]*eigenValues[1]) <= m_MaxMinorAxisLength)
      {
      // Check the direction of principal component
      VectorType principalVector = eigenMatrix[2];

      double ip = principalVector[0] * m_Normal[0] + principalVector[1] * m_Normal[1] + principalVector[2] * m_Normal[2];

      if (vnl_math_abs(ip) > cosThreshold)
        {
        PointListType::Iterator iter = sample->Begin();
        
        while (iter != sample->End())
          {
          VectorType vector = iter.GetMeasurementVector();
          typename InputImageType::PointType point;
          typename OutputImageType::IndexType index;
          point[0] = vector[0];
          point[1] = vector[1];
          point[2] = vector[2];
          output->TransformPhysicalPointToIndex (point, index);
          output->SetPixel(index, pix);
          ++ iter;
          }

        // Calculate distance between the detected needle and the closest point
        VectorType a;
        a = closestPoint - meanVector;
        double p = principalVector * a;
        double lsq_a = a * a;
        double lsq_p = p * p;
        double distance = sqrt(vnl_math_abs(lsq_a - lsq_p));
        if (closestDistance < 0 || distance < closestDistance)
          {
          closestDistance = distance;
          closestPosition = meanVector;
          closestNormal = principalVector;
          }
        }
      }
    }

  m_NeedleTransform = NeedleTransformType::New();
  m_NeedleTransform->SetIdentity();

  VectorType nx;
  VectorType ny;
  nx[0] = 1.0; nx[1] = 0.0; nx[2] = 0.0;
  ny[0] = 0.0; ny[1] = 1.0; ny[2] = 0.0;
  typedef itk::CrossHelper< VectorType > CrossType;
  CrossType cross;
  VectorType t = cross(closestNormal, nx);
  VectorType s = cross(t, closestNormal);
  
  NeedleTransformType::MatrixType matrix;
  matrix[0][0] = s[0];
  matrix[0][1] = s[1];
  matrix[0][2] = s[2];
  //matrix[0][3] = 0.0;

  matrix[1][0] = t[0];
  matrix[1][1] = t[1];
  matrix[1][2] = t[2];
  //matrix[1][3] = 0.0;

  matrix[2][0] = closestNormal[0];
  matrix[2][1] = closestNormal[1];
  matrix[2][2] = closestNormal[2];
  //matrix[2][3] = 0.0;

  //matrix[3][0] = closestPosition[0];
  //matrix[3][1] = closestPosition[1];
  //matrix[3][2] = closestPosition[2];
  //matrix[3][3] = 1.0;

  m_NeedleTransform->SetMatrix(matrix);
  m_NeedleTransform->SetTranslation(closestPosition);

    
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
