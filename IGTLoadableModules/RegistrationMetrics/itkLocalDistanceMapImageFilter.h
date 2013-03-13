/****************************************
* itkLocalDistanceMapImageFilter.h
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
#ifndef __itkLocalDistanceMapImageFilter_h
#define __itkLocalDistanceMapImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkNumericTraits.h"

namespace itk
{
/** \class LocalDistanceMapImageFilter
 * \brief computes a Hausdorff based Local Distance Map  
 *
 * Computes an image where a given pixel is the Hausdorff distance of the
 * the corresponding pixels in an input edge image.
 *
 * \ingroup IntensityImageFilters 
 */
template <class TDistance1,class TDistance2, class TOutputImage>
class ITK_EXPORT LocalDistanceMapImageFilter : 
public ImageToImageFilter<TDistance1, TOutputImage>
{
public:

  /** Extract dimension from input and output image. */
  itkStaticConstMacro(Distance1Dimension, unsigned int,
                      TDistance1::ImageDimension);
  itkStaticConstMacro(Distance2Dimension, unsigned int,
                      TDistance2::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  /** Convenient typedefs for simplifying declarations. */
  typedef TDistance1 Distance1Type;
  typedef TDistance2 Distance2Type;
  typedef TOutputImage OutputImageType;

  /** Standard class typedefs. */
  typedef LocalDistanceMapImageFilter Self;
  typedef ImageToImageFilter< Distance1Type, OutputImageType> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(LocalDistanceMapImageFilter, ImageToImageFilter);
  

  /** Image typedef support. */
  typedef typename Distance1Type::PixelType Distance1PixelType;
  typedef typename Distance2Type::PixelType Distance2PixelType;

  typedef typename OutputImageType::PixelType OutputPixelType;
  
  typedef typename Distance1Type::RegionType Distance1RegionType;
  typedef typename Distance2Type::RegionType Distance2RegionType;

  typedef typename OutputImageType::RegionType OutputImageRegionType;


/** Set the input files */
  void SetInput1(const TDistance1 * distance1);
  void SetInput2(const TDistance2 * distance2);
  
  const Distance1Type * GetInput1(void);
  const Distance2Type * GetInput2(void);

 

protected:
  LocalDistanceMapImageFilter();
  virtual ~LocalDistanceMapImageFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** LocalDistanceMapImageFilter can be implemented as a multithreaded filter.
   * Therefore, this implementation provides a ThreadedGenerateData()
   * routine which is called for each processing thread. The output
   * image data is allocated automatically by the superclass prior to
   * calling ThreadedGenerateData().  ThreadedGenerateData can only
   * write to the portion of the output image specified by the
   * parameter "outputRegionForThread"
   *
   */
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                            ThreadIdType threadId );

private:
  LocalDistanceMapImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};
  
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLocalDistanceMapImageFilter.txx"
#endif

#endif

