/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageDirectionalConstIteratorWithIndex.h,v $
  Language:  C++
  Date:      $Date: 2005/08/16 18:04:33 $
  Version:   $Revision: 1.10 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageDirectionalConstIteratorWithIndex_h
#define __itkImageDirectionalConstIteratorWithIndex_h

#include "itkImageConstIteratorWithIndex.h"
#include "itkFixedArray.h"

namespace itk
{

/** \class ImageDirectionalConstIteratorWithIndex
 * \brief A multi-dimensional
 *
 * Iterator intended to be used in the FastSweeping algorithm.
 * 
 * \code
 * 
 *  IteratorType it( image, image->GetRequestedRegion() );
 *
 *  it.GoToBegin();
 *
 *  while( ! it.IsAtEnd() ) 
 *    {
 *    while( ! it.IsAtEndOfDirection() ) 
 *      {  
 *      it.Set( 100.0 + it.Get() );
 *      ++it;
 *      }
 *    it.NextDirection();
 *    }
 *
 * \endcode
 *
  *
 * \endcode
 *
 * \par MORE INFORMATION
 *
 *
 * For a complete description of the ITK Image Iterators and their API, please
 * see the Iterators chapter in the ITK Software Guide.  The ITK Software Guide
 * is available in print and as a free .pdf download from http://www.itk.org.
 *
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
 * \sa ImageDirectionalConstIterator \sa ImageDirectionalConstIteratorWithIndex 
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
class ITK_EXPORT ImageDirectionalConstIteratorWithIndex : public ImageConstIteratorWithIndex<TImage>
{
public:
  /** Standard class typedefs. */
  typedef ImageDirectionalConstIteratorWithIndex Self;
  typedef ImageConstIteratorWithIndex<TImage>  Superclass;
  
  /** Index typedef support. While this was already typdef'ed in the superclass
   * it needs to be redone here for this subclass to compile properly with gcc.
   * Note that we have to rescope Index back to itk::Index to that is it not
   * confused with ImageIterator::Index. */
  typedef typename TImage::IndexType  IndexType;

  /** Image typedef support. While this was already typdef'ed in the superclass
   * it needs to be redone here for this subclass to compile properly with gcc.
   * Note that we have to rescope Image back to itk::Image to that is it not
   * confused with ImageIterator::Image. */
  typedef TImage ImageType;

  itkStaticConstMacro(ImageDimension, unsigned int, TImage::ImageDimension);

  /** PixelContainer typedef support. Used to refer to the container for
   * the pixel data. While this was already typdef'ed in the superclass
   * it needs to be redone here for this subclass to compile properly with gcc. */
  typedef typename TImage::PixelContainer PixelContainer;
  typedef typename PixelContainer::Pointer PixelContainerPointer;
  
  /** Region typedef support. While this was already typdef'ed in the superclass
   * it needs to be redone here for this subclass to compile properly with gcc.
   * Note that we have to rescope Region back to itk::ImageRegion so that is
   * it not confused with ImageIterator::Index. */
  typedef typename TImage::RegionType RegionType;

  /** Default constructor. Needed since we provide a cast constructor. */
  ImageDirectionalConstIteratorWithIndex() : ImageConstIteratorWithIndex<TImage>() {};
  
  /** Constructor establishes an iterator to walk a particular image and a
   * particular region of that image. */
  ImageDirectionalConstIteratorWithIndex( const TImage *ptr,
                                          const RegionType& region );

  /** Constructor that can be used to cast from an ImageIterator to an
   * ImageDirectionalConstIteratorWithIndex. Many routines return an ImageIterator but for a
   * particular task, you may want an ImageDirectionalConstIteratorWithIndex.  Rather than
   * provide overloaded APIs that return different types of Iterators, itk
   * returns ImageIterators and uses constructors to cast from an
   * ImageIterator to a ImageDirectionalConstIteratorWithIndex. */
  ImageDirectionalConstIteratorWithIndex( const ImageConstIteratorWithIndex<TImage> &it)
    { this->ImageConstIteratorWithIndex<TImage>::operator=(it); }

  /** Increment (prefix) the fastest moving dimension of the iterator's index.
   * This operator will constrain the iterator within the region (i.e. the
   * iterator will automatically wrap from the end of the row of the region
   * to the beginning of the next row of the region) up until the iterator
   * tries to moves past the last pixel of the region.  Here, the iterator
   * will be set to be one pixel past the end of the region.
   * \sa operator-- */
  Self & operator++();


  void NextDirection();
  bool IsAtEndOfDirection();
  bool IsAtEnd();
  void GoToBegin();

  private:

    FixedArray< int, ImageDimension >  m_Directions;

    IndexType        m_UnchangingBeginIndex;
    IndexType        m_UnchangingEndIndex;
    unsigned long    m_BinaryHelper;

};

/** \example itkImageIteratorTest.cpp */
/** \example itkIteratorTests.cpp */

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageDirectionalConstIteratorWithIndex.txx"
#endif

#endif 

