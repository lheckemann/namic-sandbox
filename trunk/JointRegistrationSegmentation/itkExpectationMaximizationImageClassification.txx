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
#ifndef __itkExpectationMaximizationAlgorithm_txx
#define __itkExpectationMaximizationAlgorithm_txx

#include "itkExpectationMaximizationImageClassification.h"
#include "itkImageRegionIterator.h"

namespace itk {

namespace Statistics {


template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
ExpectationMaximizationImageClassification< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::ExpectationMaximizationImageClassification()
{
   m_Interpolator = InterpolatorType::New();
}



template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
ExpectationMaximizationImageClassification< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::~ExpectationMaximizationImageClassification()
{
}




template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::AddIntensityDistributionDensity( 
              const GaussianDensityFunctionType * gaussian,
              ProportionType proportion )
{
   m_ClassIntensityDistributions.push_back( gaussian );
   m_ClassProportions.push_back( proportion );
}





template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::Update()
{
   this->GenerateData();
}




template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::GenerateData()
{

  this->Initialize();

  unsigned long int i = 0;

  while( i < m_MaximumNumberOfIterations-1 )
   {
   this->ComputeExpectation();
   this->ComputeMaximization();
   }
  this->ComputeExpectation();
  this->ComputeLabelMap();
}




 

template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::ComputeMaximization()
{
  this->ComputeRegistrationBetweenPriorsAndWeights();
  this->ComputeInhomogeneityCorrection();
}


  

template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::ComputeRegistrationBetweenPriorsAndWeights()
{
}

 
  

template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::ComputeInhomogeneityCorrection()
{
}

 

template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::ComputeLabelMap()
{
}

 
template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::ComputeExpectation()
{

  typedef itk::ImageRegionIterator< WeightsImageType >        WeightsIterator;
  typedef itk::ImageRegionConstIterator< PriorsImageType >    PriorsImageIterator;
  typedef itk::ImageRegionConstIterator< LogImageType >       CorrectedImageIterator;
   
  WeightsIterator        witr( m_WeightsImage,   m_WeightsImage->GetBufferedRegion() );
  CorrectedImageIterator citr( m_CorrectedLogImage, m_CorrectedLogImage->GetBufferedRegion() );

  witr.GoToBegin();
  citr.GoToBegin();
   
  typedef typename WeightsImageType::IndexType    WeightsIndexType;
  typedef typename WeightsImageType::PixelType    WeightsPixelType;
  typedef typename LogImageType::PixelType        CorrectedPixelType;

  WeightsIndexType index;
  WeightsPixelType weights;

  const unsigned int numberOfClasses = m_ClassPriorImage->GetVectorLength();

  while( witr.IsAtEnd() )
    {
    index = witr.GetIndex();
    
    typename WeightsImageType::PointType inputPoint;
    m_WeightsImage->TransformIndexToPhysicalPoint( index, inputPoint );

    typename TransformType::OutputPointType transformedPoint = 
                    this->m_Transform->TransformPoint( inputPoint );

    if( this->m_Interpolator->IsInsideBuffer( transformedPoint ) )
      {
      const PriorsPixelType prior  = 
             this->m_Interpolator->Evaluate( transformedPoint );
      const CorrectedPixelType correctedInputPixel = citr.Get();
      for( unsigned int i=0; i<numberOfClasses; i++)
        {
        weights[i] = prior[i] * m_ClassProportions[i] *
                     m_ClassIntensityDistributions[i]->Evaluate( 
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
ExpectationMaximizationImageClassification< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::Initialize()
{

   if( !m_InputImage )
     {
     itkExceptionMacro("Input image has not been connected. Please use SetInput()");
     }

   if( !m_Transform )
     {
     itkExceptionMacro("Transform has not been connected");
     }

   if( !m_ClassPriorImage )
     {
     itkExceptionMacro("Priors image has not been connected. Please use SetClassPrior()");
     }

   m_WeightsImage = WeightsImageType::New();

   m_WeightsImage->CopyInformation( m_InputImage );
   m_WeightsImage->SetVectorLength( m_ClassPriorImage->GetVectorLength() ); 
   m_WeightsImage->Allocate();


   // Check the input image for negative values
   typedef itk::ImageRegionConstIterator< InputImageType >    InputImageIterator;
   
   InputImageIterator  iitr( m_InputImage,   m_InputImage->GetBufferedRegion() );

   const unsigned int numberOfComponents = m_InputImage->GetVectorLength();

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

   m_LogInputImage->CopyInformation( m_InputImage );
   m_LogInputImage->SetVectorLength( m_InputImage->GetVectorLength() );
   m_LogInputImage->Allocate();
   
   typedef itk::ImageRegionIterator< LogImageType >    LogImageIterator;

   LogImageIterator litr( m_LogInputImage, m_LogInputImage->GetBufferedRegion() );

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
   m_CorrectedLogImage->CopyInformation( m_LogInputImage );
   m_CorrectedLogImage->SetVectorLength( m_LogInputImage->GetVectorLength() );
   m_CorrectedLogImage->Allocate();
   
   LogImageIterator citr( m_CorrectedLogImage, m_CorrectedLogImage->GetBufferedRegion() );

   litr.GoToBegin();
   citr.GoToBegin();

   while( litr.IsAtEnd() )
     {
     citr.Set( litr.Get() );
     ++litr;
     ++citr;
     }
 


}



 

template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::SetInput( const InputImageType * inputImage )
{
   m_InputImage = inputImage;
}


 

template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::SetClassPrior( const PriorsImageType * priorImage )
{
   m_ClassPriorImage = priorImage;
}





} // end namespace Statistics

} // end namespace itk


#endif

