#ifndef _itkFastSweepingImageFilter_txx
#define _itkFastSweepingImageFilter_txx

#include <iostream>

#include "itkFastSweepingImageFilter.h"

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
 *  Compute Arrival Times and Arrival Vectors
 */
template <class TInputImage,class TOutputImage>
void 
FastSweepingImageFilter<TInputImage,TOutputImage>
::GenerateData() 
{

  this->PrepareData();

  // Specify images and regions.
  InputImagePointer speedImage       =  this->GetInput();
  typename OutputImageType::RegionType region = speedImage->GetRequestedRegion();
  InputIteratorType speedImageIt( speedImage, region );

  OutputImagePointer arrivalTimes    =  this->GetArrivalTimes();
  OutputIteratorType arrivalTimesIt( arrivalTimes, region );

  VectorImagePointer arrivalVectors  =  this->GetArrivalVectors();
  VectorIteratorType arrivalVectorsIt( arrivalVectors, region );  
  typename VectorImageType::PixelType vectorPixel;
  
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
  typename InputImageType::IndexType start = region.GetIndex();
  typename InputImageType::SizeType size = region.GetSize();

  typename InputImageType::RegionType region2;
  typename InputImageType::IndexType start2;
  typename InputImageType::SizeType size2;

  const unsigned int radius = 1;

  size2[0] = size[0] - 2 * radius;
  size2[1] = size[1] - 2 * radius;
  size2[2] = size[2] - 2 * radius;

  start2[0] = start[0] + radius;
  start2[1] = start[1] + radius;
  start2[2] = start[2] + radius;

  region2.SetSize( size2 );
  region2.SetIndex( start2 );

  InputDirConstIteratorType speedImageDirIt( speedImage, region2 );
  OutputDirIteratorType arrivalTimesDirIt( arrivalTimes, region2 );
  VectorDirIteratorType arrivalVectorsDirIt( arrivalVectors, region2 );

  speedImageDirIt.SetRadius( radius );
  speedImageDirIt.GoToBegin();
  arrivalTimesDirIt.SetRadius( radius );
  arrivalTimesDirIt.GoToBegin();
  arrivalVectorsDirIt.SetRadius( radius );
  arrivalVectorsDirIt.GoToBegin();
  while( !speedImageDirIt.IsAtEnd() )
    {
    while( !speedImageDirIt.IsAtEndOfDirection() )
      {
      InputPixelType neighborValue = speedImageDirIt.Get();

      // Access the neighbor pixels
      for(unsigned int k=0; k < 27; k++)
        {
        InputPixelType neighborValue = speedImageDirIt.GetNeighborPixel(k);
        }
      arrivalTimesDirIt.Set( 25 );
//      arrivalVectorsDirIt.Set( speedImageDirIt.GetNeighborPixel(13) );

      ++speedImageDirIt;
      ++arrivalTimesDirIt;
      ++arrivalVectorsDirIt;
      }
    speedImageDirIt.NextDirection();
    arrivalTimesDirIt.NextDirection();
    arrivalVectorsDirIt.NextDirection();
    }

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
