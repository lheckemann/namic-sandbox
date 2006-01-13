/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageRegistrationMethod2.h,v $
  Language:  C++
  Date:      $Date: 2005/11/01 21:57:22 $
  Version:   $Revision: 1.19 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMRIInhomogeneityEstimator_txx
#define __itkMRIInhomogeneityEstimator_txx

#include "itkMRIInhomogeneityEstimator.h"

namespace itk {

namespace Statistics {


template < class TInputImage, class TInhomogeneityPrecisionType >
MRIInhomogeneityEstimator< TInputImage, TInhomogeneityPrecisionType >
::MRIInhomogeneityEstimator() :
    m_NumberOfClasses( 0 ),
    m_StructureIntensityDistributionContainer( NULL )
{
  // membership image and multi-component MR image
  this->SetNumberOfRequiredInputs(2); 
  this->SetNumberOfRequiredOutputs(1);
}

template < class TInputImage, class TInhomogeneityPrecisionType >
MRIInhomogeneityEstimator< TInputImage, TInhomogeneityPrecisionType >
::~MRIInhomogeneityEstimator()
{
}

template < class TInputImage, class TInhomogeneityPrecisionType >
void
MRIInhomogeneityEstimator< TInputImage, TInhomogeneityPrecisionType >
::SetMembershipImage(const MembershipImageType* image) 
{ 
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(1, 
                                   const_cast< MembershipImageType* >( image ) );
}

template < class TInputImage, class TInhomogeneityPrecisionType >
const MRIInhomogeneityEstimator< TInputImage, TInhomogeneityPrecisionType >
::MembershipImageType*
MRIInhomogeneityEstimator< TInputImage, TInhomogeneityPrecisionType >
::GetMembershipImage() const
{
  if (this->GetNumberOfInputs() < 2)
    {
    return 0;
    }
  
  return static_cast<const MembershipImageType * >
    (this->ProcessObject::GetInput(1) );
}

template < class TInputImage, class TInhomogeneityPrecisionType >
void
MRIInhomogeneityEstimator< TInputImage, TInhomogeneityPrecisionType >
::GenerateData()
{
  InputImageType *input = this->GetInput();
  MembershipImageType *weights = this->GetMembershipImage();
  
  if (!input || !weights) 
    {
    return;
    }
  
  this->m_NumberOfClasses = this->m_StructureIntensityDistributionContainer->Size();

  if (!this->m_StructureIntensityDistributionContainer)
    {
    this->InitializeStructureIntensityDistributions();
    }

  if (this->m_NumberOfClasses !=
        this->GetInput()->GetVectorLength() )
    {
    itkExceptionMacro( << "Number of channels in input must be equal to number "
        << "of gaussian functions supplied");
    }
  
  if (  this->m_NumberOfClasses !=
        this->GetMembershipImage()->GetVectorLength() )
    {
    itkExceptionMacro( << "Number of channels in input must be equal to number "
        << "of gaussian functions supplied");
    }

  this->AllocateOutputs();

  typedef itk::ImageRegionConstIterator< 
                              InputImageType > ConstInputIteratorType;
  typedef itk::ImageRegionConstIterator< 
                              MembershipImageType > ConstMembershipIteratorType
  typedef itk::ImageRegionConstIterator< 
                              OutputImageType > OutputImageIteratorType;
  typedef itk::ImageRegionConstIterator< 
                   InverseCovarianceImageType > InverseCovarianceImageIteratorType;

  // Iterator over the input image 
  OutputImageType *output = this->GetOutput();
  ConstInputIteratorType  cIit( input, input->GetBufferedRegion() );
  ConstMembershipIteratorType cWit( weights, weights->GetBufferedRegion() );
  OutputImageIteratorType cOit( output, output->GetBufferedRegion() );
 
  typedef VectorContainer< 
    unsigned int, CovarianceMatrixType >    InverseCovarianceContainerType;
  InverseCovarianceContainerType::Pointer inverseCovarianceContainer = 
     InverseCovarianceContainerType::New();
  StructureIntensityDistributionContainerType inverseCovarianceContainer( 
                                                 this->m_NumberOfClasses );
  
  // Precompute the inverses of the covariances of the Gaussian density 
  // functions
  for (unsigned int i=0; i<this->m_NumberOfClasses; i++)
    {
    inverseCovarianceContainer->InsertElement( i, 
      m_StructureIntensityDistributionContainer->GetElement(
                          i)->GetCovariance()->GetInverse());
    }

  // Allocate an image of smoothed inverse covariances
  this->AllocateImageOfInverseCovariances();
  // Store the weighted inverse covariances of each pixel in this image. 

  InverseCovarianceImageIteratorType cInvCovIt( 
      this->m_InverseCovarianceImage, 
      this->InverseCovarianceImage->GetLargestPossibleRegion() );

  cIit.GoToBegin();
  cWit.GoToBegin();
  cOit.GoToBegin();
  cInvCovIt.GoToBegin();
  
  // Iterate over the multi-component MR image
  while (!cIit.IsAtEnd())
    {
    MembershipImageType::PixelType membershipPixel = cWit.Get();
    InputImageType::PixelType inputPixel = cIit.Get();
    
    // For each class ...
    for (unsigned int classIdx = 0; classIdx < this->m_NumberOfClasses; classIdx++)
      {
      cInvCovIt.Set( cInvCovIt.Get() + inverseCovarianceContainer->GetElement(
         classIdx)->GetCovariance() * membershipPixel[classIdx] ); 
      cOit.Set(membershipPixel[classIdx] * 
       inverseCovarianceContainer->GetElement(
         classIdx)->GetCovariance() * (inputPixel - 
           m_StructureIntensityDistributionContainer->GetElement(
                                            classIdx)->GetMean()));
      }
    ++cOit;
    ++cWit;
    ++cIit;
    }


    for (int l=0; l< NumberOfStructures; l++) {
      for (int m=0; m< NumInputImages; m++) {  
        for (int n=0; n< NumInputImages; n++) {
          temp =  *w_m[l] * float(InverseLogCov[l][m][n]);
          r_m[m](i,k,j)     += temp * ((*InputVector)[n] - float(LogMu[l][n]));
          if (n <= m) iv_m(m,n,i,k,j) += temp;
        }
      }
      
      w_m[l]++;
    }
 
}

template < class TInputImage, class TInhomogeneityPrecisionType >
void
MRIInhomogeneityEstimator< TInputImage, TInhomogeneityPrecisionType >
::AllocateImageOfInverseCovariances()
{
  // Allocate the image storing the inverse covariances for each pixel.
  // The image will have the same dimensions as the input image and will have 
  // a nxn matrix at each pixel where n is the number of channels in 
  // the multi-component MR image.

  this->m_InverseCovarianceImage = InverseCovarianceImageType::New(); 
  this->m_InverseCovarianceImage->CopyInformation( this->GetInput() );
  this->m_InverseCovarianceImage->Allocate();
  InverseCovarianceImageType::PixelType p( 
    this->GetInput()->GetVectorLength(), this->GetInput()->GetVectorLength());
  p->Fill( 0.0 );
  this->m_InverseCovarianceImage->FillBuffer(p);

}

template < class TInputImage, class TInhomogeneityPrecisionType >
void
MRIInhomogeneityEstimator< TInputImage, TInhomogeneityPrecisionType >
::SetIntensityDistributions( StructureIntensityDistributionType 
                                                * densityFunctionContainer )
{
  this->m_NumberOfClasses = densityFunctionContainer->Size();
  this->m_StructureIntensityDistributionContainer = densityFunctionContainer;
  this->Modified();
}

/** Give reasonable defaults to the structure intensity distributions */
template < class TInputImage, class TInhomogeneityPrecisionType >
void
MRIInhomogeneityEstimator< TInputImage, TInhomogeneityPrecisionType >
::InitializeStructureIntensityDistributions()
{
  // TODO: Initialize the gaussians from KMeans or something...
  //
  // Typedefs for the KMeans filter, Covariance calculator...
}



}

}

#endif

