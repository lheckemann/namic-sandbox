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
#ifndef __itkJointRegistrationSegmentationAlgorithm_txx
#define __itkJointRegistrationSegmentationAlgorithm_txx

#include "itkJointRegistrationSegmentationAlgorithm.h"
#include "itkImageRegionIterator.h"

namespace itk {

namespace Statistics {


template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
JointRegistrationSegmentationAlgorithm< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::JointRegistrationSegmentationAlgorithm()
{
   this->m_Interpolator = InterpolatorType::New();
   this->m_RegistrationMethod = RegistrationMethodType::New();
}



template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
JointRegistrationSegmentationAlgorithm< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::~JointRegistrationSegmentationAlgorithm()
{
}


 

template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
JointRegistrationSegmentationAlgorithm< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::ComputeMaximization()
{
  this->ComputeRegistrationBetweenPriorsAndWeights();
  this->ComputeInhomogeneityCorrection();
}


  

template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
JointRegistrationSegmentationAlgorithm< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::ComputeRegistrationBetweenPriorsAndWeights()
{

   this->m_RegistrationMethod->SetFixedImage(  this->m_WeightsImage    );
   this->m_RegistrationMethod->SetMovingImage( this->m_ClassPriorImage );

   this->m_RegistrationMethod->Update();

   this->m_Transform = this->m_RegistrationMethod->GetTransform();
   
}

 
  

template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
JointRegistrationSegmentationAlgorithm< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::ComputeInhomogeneityCorrection()
{
}


 
template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
JointRegistrationSegmentationAlgorithm< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::ComputeExpectation()
{

  typedef itk::ImageRegionIterator< WeightsImageType >        WeightsIterator;
  typedef itk::ImageRegionConstIterator< LogImageType >       CorrectedImageIterator;
   
  WeightsIterator        witr( this->m_WeightsImage,   this->m_WeightsImage->GetBufferedRegion() );
  CorrectedImageIterator citr( this->m_CorrectedLogImage, this->m_CorrectedLogImage->GetBufferedRegion() );

  witr.GoToBegin();
  citr.GoToBegin();
   
  typedef typename WeightsImageType::IndexType    WeightsIndexType;
  typedef typename WeightsImageType::PixelType    WeightsPixelType;
  typedef typename LogImageType::PixelType        CorrectedPixelType;

  WeightsIndexType index;
  WeightsPixelType weights;

  const unsigned int numberOfClasses = this->GetNumberOfClasses();

  while( witr.IsAtEnd() )
    {
    index = witr.GetIndex();
    
    typename WeightsImageType::PointType inputPoint;
    this->m_WeightsImage->TransformIndexToPhysicalPoint( index, inputPoint );

    typename TransformType::OutputPointType transformedPoint = 
                    this->m_Transform->TransformPoint( inputPoint );

    if( this->m_Interpolator->IsInsideBuffer( transformedPoint ) )
      {
      const PriorsPixelType prior  = 
             this->m_Interpolator->Evaluate( transformedPoint );
      const CorrectedPixelType correctedInputPixel = citr.Get();
      for( unsigned int i=0; i<numberOfClasses; i++)
        {
        weights[i] = 
              prior[i] * 
              this->m_ClassProportions->GetElement(i) *
              this->m_ClassIntensityDistributions->GetElement(i)->Evaluate( 
                                                          correctedInputPixel );
        }
      witr.Set( weights );
      }

    ++witr;
    ++citr;
    }

}



 

template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
JointRegistrationSegmentationAlgorithm< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::Initialize()
{

   this->Superclass::Initialize();

   if( !this->m_Transform )
     {
     itkExceptionMacro("Transform has not been connected");
     }

   this->m_Interpolator->SetInputImage( this->m_ClassPriorImage );

   const unsigned int numberOfClasses =  this->m_ClassPriorImage->GetVectorLength();

   this->m_WeightsImage = WeightsImageType::New();

   this->m_WeightsImage->CopyInformation( this->GetObservations() );
   this->m_WeightsImage->SetVectorLength( numberOfClasses ); 
   this->m_WeightsImage->Allocate();


   // Check the input image for negative values
   typedef itk::ImageRegionConstIterator< InputImageType >    InputImageIterator;
   
   const InputImageType * inputImage = this->GetObservations();

   InputImageIterator  iitr( inputImage,   inputImage->GetBufferedRegion() );

   const unsigned int numberOfComponents = inputImage->GetVectorLength();

   iitr.GoToBegin();
   while( iitr.IsAtEnd() )
     {
     InputPixelType pixel = iitr.Get();
     for(unsigned int i=0; i<numberOfComponents; i++)
       {
       if( pixel[i] < 0 )
         {
         itkExceptionMacro("Input image contains negative values");
         }
       }
     ++iitr;
     }
 

   //
   // Compute the Log ( pixel + 1 )
   //
   this->m_LogInputImage->CopyInformation( inputImage );
   this->m_LogInputImage->SetVectorLength( inputImage->GetVectorLength() );
   this->m_LogInputImage->Allocate();
   
   typedef itk::ImageRegionIterator< LogImageType >    LogImageIterator;

   LogImageIterator litr( this->m_LogInputImage, this->m_LogInputImage->GetBufferedRegion() );

   iitr.GoToBegin();
   litr.GoToBegin();

   LogPixelType logPixel;
   
   while( iitr.IsAtEnd() )
     {
     InputPixelType pixel = iitr.Get();
     for(unsigned int i=0; i<numberOfComponents; i++)
       {
       logPixel[i] = log( pixel[i] + 1.0 );
       }
     litr.Set( logPixel );
     ++litr;
     ++iitr;
     }
 

   //
   //  Copy the log image into the Corrected Log image.
   //
   this->m_CorrectedLogImage->CopyInformation( this->m_LogInputImage );
   this->m_CorrectedLogImage->SetVectorLength( this->m_LogInputImage->GetVectorLength() );
   this->m_CorrectedLogImage->Allocate();
   
   LogImageIterator citr( this->m_CorrectedLogImage, this->m_CorrectedLogImage->GetBufferedRegion() );

   litr.GoToBegin();
   citr.GoToBegin();

   while( litr.IsAtEnd() )
     {
     citr.Set( litr.Get() );
     ++litr;
     ++citr;
     }
 


 
    //
    // Setup the registration Module...
    //



    
    //
    // Setup the Image Inhomogeneity Correction Module...
    //

}




} // end namespace Statistics

} // end namespace itk


#endif

