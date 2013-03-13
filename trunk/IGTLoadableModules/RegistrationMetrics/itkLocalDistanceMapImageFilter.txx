/****************************************
* itkLocalDistanceMapImageFilter.txx
*
* Authors: Eric Billet, Andriy Fedorov 
* PI: Nikos Chrisochoides
***********************************/
/*
Copyright (c) 2008 College of William and Mary
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

   * Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the
distribution.
   * Neither the name of the College of William and Mary nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _itkLocalDistanceMapImageFilter_txx
#define _itkLocalDistanceMapImageFilter_txx
#include "itkLocalDistanceMapImageFilter.h"

#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhoodInnerProduct.h"
#include "itkImageRegionIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"
#include "itkOffset.h"
#include "itkProgressReporter.h"

namespace itk
{

template<class TDistance1,class TDistance2, class TOutputImage>

LocalDistanceMapImageFilter<TDistance1,TDistance2,TOutputImage>
::LocalDistanceMapImageFilter(){
// this filter requires two input images
  this->SetNumberOfRequiredInputs( 2 );
}


template<class TDistance1,class TDistance2, class TOutputImage>
void
LocalDistanceMapImageFilter<TDistance1,TDistance2,TOutputImage>
::SetInput1( const TDistance1 * distance1 )
{
  this->SetNthInput(0, const_cast<TDistance1 *>( distance1 ) );      
}

template<class TDistance1,class TDistance2, class TOutputImage>
void
LocalDistanceMapImageFilter<TDistance1,TDistance2,TOutputImage>
::SetInput2( const TDistance2 * distance2 )
{
  this->SetNthInput(1, const_cast<TDistance2 *>( distance2 ) );      
}

template<class TDistance1,class TDistance2, class TOutputImage>
const 
typename LocalDistanceMapImageFilter<TDistance1,TDistance2,TOutputImage>
::Distance1Type * LocalDistanceMapImageFilter<TDistance1,TDistance2,TOutputImage>::GetInput1()
{
  return static_cast< const TDistance1 * >
    (this->ProcessObject::GetInput(0));
}

template<class TDistance1,class TDistance2, class TOutputImage>
const 
typename LocalDistanceMapImageFilter<TDistance1,TDistance2,TOutputImage>
::Distance2Type * LocalDistanceMapImageFilter<TDistance1,TDistance2,TOutputImage>::GetInput2()
{
  return static_cast< const TDistance2 * >
    (this->ProcessObject::GetInput(1));
}


template<class TDistance1,class TDistance2, class TOutputImage>
void
LocalDistanceMapImageFilter<TDistance1,TDistance2,TOutputImage>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       ThreadIdType threadId)
{

  /** Allocate output*/
  typename OutputImageType::Pointer outputImage = this->GetOutput();

  typename  Distance1Type::ConstPointer distance1  = this->GetInput1();
  typename  Distance1Type::ConstPointer distance2  = this->GetInput2();

  /**create iterators for the 3 images (2 distance transforms, output image)*/
    ImageRegionConstIterator<Distance1Type> DistanceIt(distance1, outputRegionForThread);
    ImageRegionConstIterator<Distance2Type> DistanceIt2(distance2, outputRegionForThread);
    ImageRegionIterator<OutputImageType> OutputIt(outputImage, outputRegionForThread);

    ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());

        /**Iterate through the images, determining the local distance map. Value of a given pixel in the distance map is:
           |1A(x) - 1B(x)| x max(d(x,A),d(x,B)) , where 1A() is a function which has value 1 if A(x) is non-zero and 0
           otherwise.  d(x,A) represents the distance from x to the nearest point in A*/
    for(DistanceIt.GoToBegin(),DistanceIt2.GoToBegin(),OutputIt.GoToBegin(); !DistanceIt.IsAtEnd(); ++DistanceIt, ++DistanceIt2, ++OutputIt){
        //find LD values
        double Edge1 = 0;
        double Edge2 = 0;

        if(DistanceIt.Get())
          Edge1 = 1;
        if(DistanceIt2.Get())
          Edge2=1;

        if(DistanceIt.Get()>DistanceIt2.Get())
          OutputIt.Set(fabs(Edge1-Edge2)*DistanceIt.Get());
        else
          OutputIt.Set(fabs(Edge2-Edge1)*DistanceIt2.Get());
        progress.CompletedPixel();
    }
}

/**
 * Standard "PrintSelf" method
 */
template<class TDistance1,class TDistance2, class TOutputImage>
void
LocalDistanceMapImageFilter<TDistance1,TDistance2,TOutputImage>
::PrintSelf(
  std::ostream& os, 
  Indent indent) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << std::endl;

}

} // end namespace itk

#endif

