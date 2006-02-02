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
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"

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
const typename MRIInhomogeneityEstimator< TInputImage, TInhomogeneityPrecisionType >
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

  typedef ImageRegionConstIterator< 
                         InputImageType > ConstInputIteratorType;
  typedef ImageRegionConstIterator< 
                         MembershipImageType > ConstMembershipIteratorType;
  typedef ImageRegionConstIterator< 
                         OutputImageType > OutputImageIteratorType;
  typedef ImageRegionIterator< 
                   InverseCovarianceImageType > InverseCovarianceImageIteratorType;

  // Iterator over the input image 
  OutputImageType *output = this->GetOutput();
  ConstInputIteratorType  cIit( input, input->GetBufferedRegion() );
  ConstMembershipIteratorType cWit( weights, weights->GetBufferedRegion() );
  OutputImageIteratorType cOit( output, output->GetBufferedRegion() );
 
  typedef VectorContainer< 
    unsigned int, CovarianceMatrixType >    InverseCovarianceContainerType;
  typename InverseCovarianceContainerType::Pointer inverseCovarianceContainer = 
     InverseCovarianceContainerType::New();
  inverseCovarianceContainer->Reserve( this->m_NumberOfClasses );
  
  // Precompute the inverses of the covariances of the Gaussian density 
  // functions
  for (unsigned int i=0; i<this->m_NumberOfClasses; i++)
    {
    inverseCovarianceContainer->InsertElement( i, 
      m_StructureIntensityDistributionContainer->GetElement(
                          i)->GetCovariance()->GetInverse());
    }

  // Allocate an image of smoothed inverse covariances. This is needed to 
  // estimate H, which will be used to calculate the bias field.
  //
  // This image will store the weighted inverse covariances of each pixel in 
  // the image. This image will have as many pixels as the input image. Each
  // pixel in this image is a covariance matrix of size m x m where m is the 
  // number of MRI images. 
  this->AllocateImageOfInverseCovariances();

  
  InverseCovarianceImageIteratorType cInvCovIt( 
      this->m_InverseCovarianceImage, 
      this->InverseCovarianceImage->GetLargestPossibleRegion() );

  cIit.GoToBegin();
  cWit.GoToBegin();
  cOit.GoToBegin();
  cInvCovIt.GoToBegin();
  
  // Iterate over the multi-component MR image
  // 
  // For now its simple without any smoothing.. 
  //
  while (!cIit.IsAtEnd())
    {
    typename MembershipImageType::PixelType membershipPixel = cWit.Get();
    typename InputImageType::PixelType inputPixel = cIit.Get();
    
    // For each class ...
    //
    // Inverse covariance image = SUM over classes [( inverse_covariance of 
    // each class ) weighted by membership for that class at this pixel]
    // 
    // Output = membershipPixel[classIdx] * 
    //      inverse_covariance_matrix for that class [mxm matrix] * 
    //      ( input_Pixel (1xm) - mean of the distribution set for this class (1xm) )
    //
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

  

  
  // TODO: Estimate H from the mean covariance of the tissue class intensities.
  // Multiply with residual to get the bias field.
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
  typename InverseCovarianceImageType::PixelType p( 
    this->GetInput()->GetVectorLength(), this->GetInput()->GetVectorLength());
  p->Fill( 0.0 );
  this->m_InverseCovarianceImage->FillBuffer(p);

}

template < class TInputImage, class TInhomogeneityPrecisionType >
void
MRIInhomogeneityEstimator< TInputImage, TInhomogeneityPrecisionType >
::SetIntensityDistributions( StructureIntensityDistributionContainerType 
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

