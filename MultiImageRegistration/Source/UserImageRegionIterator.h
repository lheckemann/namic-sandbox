/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageRegionIterator.h,v $
  Language:  C++
  Date:      $Date: 2006/06/09 12:49:25 $
  Version:   $Revision: 1.38 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __UserImageRegionIterator_h
#define __UserImageRegionIterator_h

#include "itkImageRegionIterator.h"
#include "itkImageIteratorWithIndex.h"

namespace itk
{

/** \class ImageRegionIterator
 * \brief A multi-dimensional iterator templated over image type that walks a
 * region of pixels.
 *
 * The itk::ImageRegionIterator is optimized for iteration speed and is the
 * first choice for iterative, pixel-wise operations on an image.
 * ImageRegionIterator is the least specialized of the ITK image iterator
 * classes.  ImageRegionIterator is templated over the image type, and is
 * constrained to walk only within the specified region and along a line
 * parallel to one of the coordinate axes, "wrapping" to the next line as it
 * reaches the boundary of the image.  To walk the entire image, specify the
 * region to be \c image->GetRequestedRegion().
 *
 * Most of the functionality is inherited from the ImageRegionConstIterator.
 * The current class only adds write access to image pixels.
 *
 * \par MORE INFORMATION
 * For a complete description of the ITK Image Iterators and their API, please
 * see the Iterators chapter in the ITK Software Guide.  The ITK Software Guide
 * is available in print and as a free .pdf download from http://www.itk.org.
 *
 * \example Iterators/ImageRegionIterator.cxx
 * \ingroup ImageIterators
 *
 * \sa ImageConstIterator \sa ConditionalConstIterator
 * \sa ConstNeighborhoodIterator \sa ConstShapedNeighborhoodIterator
 * \sa ConstSliceIterator  \sa CorrespondenceDataStructureIterator 
 * \sa FloodFilledFunctionConditionalConstIterator 
 * \sa FloodFilledImageFunctionConditionalConstIterator 
 * \sa FloodFilledImageFunctionConditionalIterator 
 * \sa FloodFilledSpatialFunctionConditionalConstIterator 
 * \sa FloodFilledSpatialFunctionConditionalIterator 
 * \sa ImageConstIterator \sa ImageConstIteratorWithIndex 
 * \sa ImageIterator \sa ImageIteratorWithIndex
 * \sa ImageLinearConstIteratorWithIndex  \sa ImageLinearIteratorWithIndex 
 * \sa ImageRandomConstIteratorWithIndex  \sa ImageRandomIteratorWithIndex 
 * \sa ImageRegionConstIterator \sa ImageRegionConstIteratorWithIndex 
 * \sa ImageRegionExclusionConstIteratorWithIndex 
 * \sa ImageRegionExclusionIteratorWithIndex 
 * \sa ImageRegionIterator  \sa ImageRegionIteratorWithIndex 
 * \sa ImageRegionReverseConstIterator  \sa ImageRegionReverseIterator 
 * \sa ImageReverseConstIterator  \sa ImageReverseIterator 
 * \sa ImageSliceConstIteratorWithIndex  \sa ImageSliceIteratorWithIndex 
 * \sa NeighborhoodIterator \sa PathConstIterator  \sa PathIterator 
 * \sa ShapedNeighborhoodIterator  \sa SliceIterator 
 * \sa ImageConstIteratorWithIndex */
template<typename TImage>
class ITK_EXPORT UserImageRegionIterator : public ImageRegionIterator<TImage>
{
public:
  /** Standard class typedefs. */
  typedef UserImageRegionIterator Self;
  typedef ImageRegionIterator<TImage>  Superclass;
  
   /** Types inherited from the Superclass */
  typedef typename Superclass::IndexType              IndexType;
  typedef typename Superclass::IndexValueType         IndexValueType;
  typedef typename Superclass::SizeType               SizeType;
  typedef typename Superclass::SizeValueType          SizeValueType;
  typedef typename Superclass::OffsetType             OffsetType;
  typedef typename Superclass::OffsetValueType        OffsetValueType;
  typedef typename Superclass::RegionType             RegionType;
  typedef typename Superclass::ImageType              ImageType;
  typedef typename Superclass::PixelContainer         PixelContainer;
  typedef typename Superclass::PixelContainerPointer  PixelContainerPointer;
  typedef typename Superclass::InternalPixelType      InternalPixelType;
  typedef typename Superclass::PixelType              PixelType;
  typedef typename Superclass::AccessorType           AccessorType;

  /** Default constructor. Needed since we provide a cast constructor. */
  UserImageRegionIterator()
  : ImageRegionIterator<TImage>(){};
  
  /** Constructor establishes an iterator to walk a particular image and a
   * particular region of that image. */
  UserImageRegionIterator(ImageType *ptr, const RegionType& region)
     : ImageRegionIterator<TImage>(   ptr, region ) {};

  /** Constructor that can be used to cast from an ImageIterator to an
   * ImageRegionIterator. Many routines return an ImageIterator but for a
   * particular task, you may want an ImageRegionIterator.  Rather than
   * provide overloaded APIs that return different types of Iterators, itk
   * returns ImageIterators and uses constructors to cast from an
   * ImageIterator to a ImageRegionIterator. */
  UserImageRegionIterator( const ImageIterator<TImage> &it)
     : ImageRegionIterator<TImage>(it){};
  

  /** Return the location in the 1-d array */
  const unsigned long GetBuffer() const
    {return this->m_Offset;}

  

};

} // end namespace itk

// Define instantiation macro for this template.
#define ITK_TEMPLATE_ImageRegionIterator(_, EXPORT, x, y) namespace itk { \
  _(1(class EXPORT ImageRegionIterator< ITK_TEMPLATE_1 x >)) \
  namespace Templates { typedef ImageRegionIterator< ITK_TEMPLATE_1 x > ImageRegionIterator##y; } \
  }


#if ITK_TEMPLATE_EXPLICIT
# include "Templates/UserImageRegionIterator+-.h"
#endif



// #ifndef ITK_MANUAL_INSTANTIATION
// #include "itkImageRegionIterator.txx"
// #endif

#endif 



