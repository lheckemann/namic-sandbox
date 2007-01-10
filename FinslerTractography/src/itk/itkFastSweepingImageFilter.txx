#ifndef _itkFastSweepingImageFilter_txx
#define _itkFastSweepingImageFilter_txx

#include <iostream>

#include "itkFastSweepingImageFilter.h"
#include "itkReflectiveImageRegionConstIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace itk
{


/**
 *    Constructor
 */
template <class TInputImage,class TOutputImage>
FastSweepingImageFilter<TInputImage,TOutputImage>
::FastSweepingImageFilter()
{

  this->SetNumberOfRequiredOutputs( 2 );

  OutputImagePointer arrivalTimes = OutputImageType::New();
  this->SetNthOutput( 0, arrivalTimes.GetPointer() );

  VectorImagePointer arrivalVectors = VectorImageType::New();
  this->SetNthOutput( 1, arrivalVectors.GetPointer() );

}



/**
 *  Return the arrival times Image pointer
 */
template <class TInputImage,class TOutputImage>
typename FastSweepingImageFilter<
  TInputImage,TOutputImage>::OutputImageType * 
FastSweepingImageFilter<TInputImage,TOutputImage>
::GetArrivalTimes(void)
{
  return  dynamic_cast< OutputImageType * >(
    this->ProcessObject::GetOutput(0) );
}





/**
 *  Return the arrival vectors
 */
template <class TInputImage,class TOutputImage>
typename FastSweepingImageFilter<
  TInputImage,TOutputImage>::VectorImageType * 
FastSweepingImageFilter<TInputImage,TOutputImage>
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
FastSweepingImageFilter<TInputImage,TOutputImage>
::PrepareData(void) 
{
  
  itkDebugMacro(<< "PrepareData Start");

  InputImagePointer  inputImage  = 
    dynamic_cast<const TInputImage  *>( ProcessObject::GetInput(0) );

  /* Prepare the arrival times */
  OutputImagePointer arrivalTimes = this->GetDistanceMap();

  arrivalTimes->CopyInformation( inputImage );

  arrivalTimes->SetBufferedRegion( 
    inputImage->GetBufferedRegion() );

  arrivalTimes->SetRequestedRegion( 
    inputImage->GetRequestedRegion() );

  arrivalTimes->Allocate();
  arrivalTimes->FillBuffer( itk::NumericTraits<
                            typename OutputImageType::PixelType >::max() );

  /* Prepare the arrival vectors */
  VectorImagePointer arrivalVectors = GetVectorDistanceMap();

  arrivalVectors->CopyInformation( inputImage );

  arrivalVectors->SetBufferedRegion( 
    inputImage->GetBufferedRegion() );

  arrivalVectors->SetRequestedRegion( 
    inputImage->GetRequestedRegion() );

  arrivalVectors->Allocate();

  itkDebugMacro(<< "PrepareData End");    
}




/**
 *  Compute Arrival Times and Arrival Vectors
 */
template <class TInputImage,class TOutputImage>
void 
FastSweepingImageFilter<TInputImage,TOutputImage>
::GenerateData() 
{

  this->PrepareData();

  // Specify images and regions.
  
  OutputImagePointer arrivalTimes    =  this->GetArrivalTimes();
  OutputImageType::RegionType region = arrivalTimes->GetRequestedRegion();
  OutputIteratorType arrivalTimesIt( arrivalTimes, region );

  VectorImagePointer arrivalVectors  =  this->GetArrivalVectors();
  VectorIteratorType arrivalVectorsIt( arrivalVectors, region );  
  VectorImageType::PixelType vectorPixel;
  
  itkDebugMacro (<< "Region to process " << region);

  // Test the iterators
  arrivalTimesIt.GoToBegin();
  arrivalVectorsIt.GoToBegin();
  int count = 0;
  while( !arrivalTimesIt.IsAtEnd() )
    {
    arrivalTimesIt.Set( count );
    vectorPixel.Fill( count );
    arrivalVectorsIt.Set( vectorPixel );
    ++arrivalTimesIt;
    ++arrivalVectorsIt;
    ++count;
    }

  // Instantiate the directional iterator

  itkDebugMacro(<< "GenerateData");
  
} // end GenerateData()




/**
 *  Print Self
 */
template <class TInputImage,class TOutputImage>
void 
FastSweepingImageFilter<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  
  os << indent << "FastSweeping" << std::endl;

}



} // end namespace itk

#endif
