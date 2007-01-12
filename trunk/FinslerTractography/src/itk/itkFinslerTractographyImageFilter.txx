#ifndef _itkFinslerTractographyImageFilter_txx
#define _itkFinslerTractographyImageFilter_txx

#include <iostream>

#include "itkFinslerTractographyImageFilter.h"

namespace itk
{


/**
 *    Constructor
 */
template <class TInputImage,class TOutputImage>
FinslerTractographyImageFilter<TInputImage,TOutputImage>
::FinslerTractographyImageFilter() :
  m_GradientDiffusionVectorsContainer(0)
{
  this->SetNumberOfRequiredOutputs( 2 );

  OutputImagePointer arrivalTimes = OutputImageType::New();
  this->SetNthOutput( 0, arrivalTimes.GetPointer() );

  VectorImagePointer arrivalVectors = VectorImageType::New();
  this->SetNthOutput( 1, arrivalVectors.GetPointer() );

  this->m_GradientDiffusionVectorsContainer = NULL;
}


/**
 *  Set the gradient directions
 */
template <class TInputImage,class TOutputImage>
void 
FinslerTractographyImageFilter<TInputImage,TOutputImage>
::SetGradientDiffusionVectors( GradientDiffusionVectorsContainerType *gradientDirections )
{
  this->m_GradientDiffusionVectorsContainer = gradientDirections;
}


/**
 *  Return the arrival times Image pointer
 */
template <class TInputImage,class TOutputImage>
typename FinslerTractographyImageFilter<
  TInputImage,TOutputImage>::OutputImageType * 
FinslerTractographyImageFilter<TInputImage,TOutputImage>
::GetArrivalTimes(void)
{
  return  dynamic_cast< OutputImageType * >(
    this->ProcessObject::GetOutput(0) );
}


/**
 *  Return the arrival vectors
 */
template <class TInputImage,class TOutputImage>
typename FinslerTractographyImageFilter<
  TInputImage,TOutputImage>::VectorImageType * 
FinslerTractographyImageFilter<TInputImage,TOutputImage>
::GetArrivalVectors(void)
{
  return  dynamic_cast< VectorImageType * >(
    this->ProcessObject::GetOutput(1) );
}


/**
 *  Prepare data for computation
 */
template <class TInputImage,class TOutputImage>
void 
FinslerTractographyImageFilter<TInputImage,TOutputImage>
::PrepareData(void) 
{
  itkDebugMacro(<< "PrepareData Start");

  InputImagePointer  inputImage  = 
    dynamic_cast<const TInputImage  *>( ProcessObject::GetInput(0) );

  /* Prepare the arrival times */
  OutputImagePointer arrivalTimes = this->GetArrivalTimes();

  arrivalTimes->CopyInformation( inputImage );

  arrivalTimes->SetBufferedRegion( 
    inputImage->GetBufferedRegion() );

  arrivalTimes->SetRequestedRegion( 
    inputImage->GetRequestedRegion() );

  arrivalTimes->Allocate();
  arrivalTimes->FillBuffer( itk::NumericTraits<
                            typename OutputImageType::PixelType >::max() );

  /* Prepare the arrival vectors */
  VectorImagePointer arrivalVectors = GetArrivalVectors();

  arrivalVectors->CopyInformation( inputImage );

  arrivalVectors->SetBufferedRegion( 
    inputImage->GetBufferedRegion() );

  arrivalVectors->SetRequestedRegion( 
    inputImage->GetRequestedRegion() );

  arrivalVectors->Allocate();

  itkDebugMacro(<< "PrepareData End");    
}




/**
 *  Run Finsler Tractography
 */
template <class TInputImage,class TOutputImage>
void 
FinslerTractographyImageFilter<TInputImage,TOutputImage>
::GenerateData() 
{
  this->PrepareData();

  /* Generate the Local Cost Function */

  /* Run Fast Sweeping */
  typename FastSweepingFilterType::Pointer fastSweepingFilter = FastSweepingFilterType::New();
  fastSweepingFilter->SetInput( this->GetInput() );
  fastSweepingFilter->SetGradientDiffusionVectors( this->m_GradientDiffusionVectorsContainer );
  fastSweepingFilter->Update();
  
  /* Generate the Tracts */
  // use std vector of itk points
  
} // end GenerateData()


/**
 *  Print Self
 */
template <class TInputImage,class TOutputImage>
void 
FinslerTractographyImageFilter<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  
  os << indent << "FinslerTractography: " << std::endl;
  os << indent << "  GradientDiffusionVectorsContainer: "
     << m_GradientDiffusionVectorsContainer << std::endl;
}


} // end namespace itk

#endif
