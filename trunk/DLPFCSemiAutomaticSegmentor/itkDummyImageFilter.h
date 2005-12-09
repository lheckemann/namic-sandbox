/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDummyImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/03/14 21:47:51 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkDummyImageFilter_h
#define __itkDummyImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

namespace itk
{
  
/** \class DummyImageFilter
 *
 * \brief This filter is a dummy filter.
 *
 * [add detailed documentation]
 * 
 * This filter is templated over the input image type
 * and the output image type.
 * 
 * The filter expect both images to have the same number of dimensions.
 *
 * \author John Melonakos, Georgia Tech
 *
 * \ingroup IntensityImageFilters  Multithreaded
 */


template <class TInputImage, class TOutputImage>
class ITK_EXPORT DummyImageFilter :
    public
ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef DummyImageFilter  Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage
  >  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DummyImageFilter, ImageToImageFilter);

  /** Dimension of the input image */
  itkStaticConstMacro( Dimension, unsigned int, 
                       itk::GetImageDimension< InputImageType >::ImageDimension );

  /** Input and Output image types */
  typedef typename Superclass::InputImageType   InputImageType;
  typedef typename Superclass::OutputImageType  OutputImageType;

  /** Input and Output image iterators */
  typedef itk::ImageRegionConstIterator< InputImageType > InputImageIteratorType;
  typedef itk::ImageRegionIterator< OutputImageType >     OutputImageIteratorType;

  /** Pixel types. */
  typedef typename TInputImage::PixelType  InputPixelType;
  typedef typename TOutputImage::PixelType OutputPixelType;

  /** Set filter mask */
  void SetMask();

protected:
  DummyImageFilter();
  virtual ~DummyImageFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Here is where the classification is computed.*/
  virtual void GenerateData();

private:
  DummyImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

//  MembershipFunctionContainer     m_MembershipFunctions;  
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDummyImageFilter.txx"
#endif

#endif
