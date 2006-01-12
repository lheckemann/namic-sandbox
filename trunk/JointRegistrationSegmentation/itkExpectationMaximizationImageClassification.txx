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
   this->m_Interpolator = InterpolatorType::New();
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
   this->m_ClassIntensityDistributions.push_back( gaussian );
   this->m_ClassProportions.push_back( proportion );
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

  while( i < this->m_MaximumNumberOfIterations-1 )
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

  const unsigned int numberOfClasses = this->m_ClassPriorImage->GetVectorLength();

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
        weights[i] = prior[i] * this->m_ClassProportions[i] *
                     this->m_ClassIntensityDistributions[i]->Evaluate( 
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

   if( !this->m_InputImage )
     {
     itkExceptionMacro("Input image has not been connected. Please use SetInput()");
     }

   if( !this->m_Transform )
     {
     itkExceptionMacro("Transform has not been connected");
     }

   if( !this->m_ClassPriorImage )
     {
     itkExceptionMacro("Priors image has not been connected. Please use SetClassPrior()");
     }

   this->m_Interpolator->SetInputImage( this->m_ClassPriorImage );

   this->m_WeightsImage = WeightsImageType::New();

   const unsigned int numberOfClasses =  this->m_ClassPriorImage->GetVectorLength();

   this->m_WeightsImage->CopyInformation( this->m_InputImage );
   this->m_WeightsImage->SetVectorLength( numberOfClasses ); 
   this->m_WeightsImage->Allocate();


   //
   //   Checking that the number of intensity distribution match 
   //   the number of components of the priors image.
   //
   if( this->m_ClassIntensityDistributions.size() != numberOfClasses )
     { 
     itkExceptionMacro("Number of provided Intensity Distributions does not match the number of components in the priors");
     }

   if( this->m_ClassProportions.size() != numberOfClasses )
     { 
     itkExceptionMacro("Number of classes proportions does not match the number of components in the priors");
     }


   // Check the input image for negative values
   typedef itk::ImageRegionConstIterator< InputImageType >    InputImageIterator;
   
   InputImageIterator  iitr( this->m_InputImage,   this->m_InputImage->GetBufferedRegion() );

   const unsigned int numberOfComponents = this->m_InputImage->GetVectorLength();

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
   this->m_LogInputImage->CopyInformation( this->m_InputImage );
   this->m_LogInputImage->SetVectorLength( this->m_InputImage->GetVectorLength() );
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
 


}



 

template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::SetInput( const InputImageType * inputImage )
{
   this->m_InputImage = inputImage;
}


 

template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType >
void
ExpectationMaximizationImageClassification< TImageType, TPriorPixelComponentType, TCorrectionPrecisionType >
::SetClassPrior( const PriorsImageType * priorImage )
{
   this->m_ClassPriorImage = priorImage;
}





} // end namespace Statistics

} // end namespace itk


#endif

