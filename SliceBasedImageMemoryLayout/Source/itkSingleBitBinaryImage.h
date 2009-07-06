/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSingleBitBinaryImage.h,v $
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSingleBitBinaryImage_h
#define __itkSingleBitBinaryImage_h

#include "itkImageBase.h"
#include "itkImageRegion.h"
#include "itkImportImageContainer.h"
#include "itkSingleBitBinaryImagePixelAccessor.h"
#include "itkSingleBitBinaryImagePixelAccessorFunctor.h"
#include "itkSingleBitBinaryImageNeighborhoodAccessorFunctor.h"
#include "itkPoint.h"
#include "itkFixedArray.h"
#include "itkWeakPointer.h"

namespace itk
{
/** \class SingleBitBinaryImage
 *  \brief An n-dimensional image where each pixel is represented
 *  as a single bit.
 *
 * The elements for a "normal" itk::Image are stored in a single, contiguous
 * 1-D array, where each pixel is at least one byte (8-bits). The elements for
 * this image are stored in a single bit, in groups of 4 bytes (32-bits).
 * This means that if your image size is 65x65 for example, an array of
 * size 96x96 will be allocated.
 *
 * The class is templated over the image dimension.
 * The internal pixel type is a 1-bit boolean.
 * The external pixel type is an 8-bit boolean.
 *
 * \ingroup ImageObjects
 */
template <unsigned int VImageDimension=2>
class ITK_EXPORT SingleBitBinaryImage : public ImageBase<VImageDimension>
{
public:
  /** Standard class typedefs */
  typedef SingleBitBinaryImage         Self;
  typedef ImageBase<VImageDimension>   Superclass;
  typedef SmartPointer<Self>           Pointer;
  typedef SmartPointer<const Self>     ConstPointer;
  typedef WeakPointer<const Self>      ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(SingleBitBinaryImage, ImageBase);

  /** Pixel typedef support. Used to declare pixel type in filters
   * or other operations. */
  typedef bool PixelType;

  /** Typedef alias for PixelType */
  typedef bool ValueType;

  /** Internal Pixel representation. Used to maintain a uniform API
   * with Image Adaptors and allow to keep a particular internal
   * representation of data while showing a different external
   * representation. */
  typedef unsigned long InternalPixelType;

  typedef PixelType IOPixelType;

  /** Accessor type that convert data between internal and external
   *  representations. */
  typedef SingleBitBinaryImagePixelAccessor< PixelType, InternalPixelType > AccessorType;
  typedef SingleBitBinaryImagePixelAccessorFunctor< Self >  AccessorFunctorType;

  /** Typedef for the functor used to access a neighborhood of pixel
   * pointers. */
  typedef SingleBitBinaryImageNeighborhoodAccessorFunctor< Self >
     NeighborhoodAccessorFunctorType;

  /** Dimension of the image.  This constant is used by functions that are
   * templated over image type (as opposed to being templated over pixel type
   * and dimension) when they need compile time access to the dimension of
   * the image. */
  itkStaticConstMacro(ImageDimension, unsigned int, VImageDimension);

  /** Container used to store pixels in the image. */
  typedef ImportImageContainer<unsigned long, InternalPixelType> PixelContainer;

  /** Index typedef support. An index is used to access pixel values. */
  typedef typename Superclass::IndexType       IndexType;
  typedef typename Superclass::IndexValueType  IndexValueType;

  /** Offset typedef support. An offset is used to access pixel values. */
  typedef typename Superclass::OffsetType OffsetType;

  /** Size typedef support. A size is used to define region bounds. */
  typedef typename Superclass::SizeType  SizeType;

  /** Direction typedef support. A matrix of direction cosines. */
  typedef typename Superclass::DirectionType  DirectionType;

  /** Region typedef support. A region is used to specify a subset of an image. */
  typedef typename Superclass::RegionType  RegionType;

  /** Spacing typedef support.  Spacing holds the size of a pixel.  The
   * spacing is the geometric distance between image samples. */
  typedef typename Superclass::SpacingType SpacingType;

  /** Origin typedef support.  The origin is the geometric coordinates
   * of the index (0,0). */
  typedef typename Superclass::PointType PointType;

  /** A pointer to the pixel container. */
  typedef typename PixelContainer::Pointer        PixelContainerPointer;
  typedef typename PixelContainer::ConstPointer   PixelContainerConstPointer;

  /** Offset typedef (relative position between indices) */
  typedef typename Superclass::OffsetValueType OffsetValueType;

  /** Allocate the image memory. The size of the image must
   * already be set, e.g. by calling SetRegions(). */
  void Allocate();

  /** Convenience methods to set the LargestPossibleRegion,
   *  BufferedRegion and RequestedRegion. Allocate must still be called.
   */
  void SetRegions(RegionType region)
    {
    this->SetLargestPossibleRegion(region);
    this->SetBufferedRegion(region);
    this->SetRequestedRegion(region);
    };

  void SetRegions(SizeType size)
    {
    RegionType region; region.SetSize(size);
    this->SetLargestPossibleRegion(region);
    this->SetBufferedRegion(region);
    this->SetRequestedRegion(region);
    }

  /** Restore the data object to its initial state. This means releasing
   * memory. */
  virtual void Initialize();

  /** Fill the image buffer with a value.  Be sure to call Allocate()
   * first. */
  void FillBuffer (const PixelType& value);

  /** \brief Set a pixel value.
   *
   * Allocate() needs to have been called first -- for efficiency,
   * this function does not check that the image has actually been
   * allocated yet. */
  void SetPixel(const IndexType &index, const PixelType& value)
    {
    typename Superclass::OffsetValueType offset = this->ComputeOffset(index);
    typename Superclass::OffsetValueType groupOffset = offset / 32;
    typename Superclass::OffsetValueType bitOffset = offset % 32;
    InternalPixelType bitMask = ( 0x1 << bitOffset );
    if ( value )
      {
      (*m_Buffer)[groupOffset] |= bitMask;
      }
    else
      {
      (*m_Buffer)[groupOffset] &= ~bitMask;
      }
    }

  /** \brief Get a pixel (read only version).
   *
   * For efficiency, this function does not check that the
   * image has actually been allocated yet. */
  const PixelType GetPixel(const IndexType &index) const
    {
    typename Superclass::OffsetValueType offset = this->ComputeOffset(index);
    typename Superclass::OffsetValueType groupOffset = offset / 32;
    typename Superclass::OffsetValueType bitOffset = offset % 32;
    InternalPixelType bitMask = 0x1 << bitOffset;
    return ( ((*m_Buffer)[groupOffset] & bitMask) == bitMask );
    }

  /** \brief Get a reference to a pixel (e.g. for editing).
   *
   * For efficiency, this function does not check that the
   * image has actually been allocated yet. */
  PixelType GetPixel(const IndexType &index)
    {
    typename Superclass::OffsetValueType offset = this->ComputeOffset(index);
    typename Superclass::OffsetValueType groupOffset = offset / 32;
    typename Superclass::OffsetValueType bitOffset = offset % 32;
    InternalPixelType bitMask = 0x1 << bitOffset;
    return ( ((*m_Buffer)[groupOffset] & bitMask) == bitMask );
    }

  /** \brief Access a pixel. This version can be an lvalue.
   *
   * For efficiency, this function does not check that the
   * image has actually been allocated yet. */
  PixelType operator[](const IndexType &index)
    { return this->GetPixel(index); }

  /** \brief Access a pixel. This version can only be an rvalue.
   *
   * For efficiency, this function does not check that the
   * image has actually been allocated yet. */
  const PixelType operator[](const IndexType &index) const
     { return this->GetPixel(index); }

  /** Return a pointer to the beginning of the buffer.  This is used by
   * the image iterator class. */
  InternalPixelType *GetBufferPointer()
    { return m_Buffer ? m_Buffer->GetBufferPointer() : 0; }
  const InternalPixelType *GetBufferPointer() const
    { return m_Buffer ? m_Buffer->GetBufferPointer() : 0; }

  /** Return a pointer to the container. */
  PixelContainer* GetPixelContainer()
    { return m_Buffer.GetPointer(); }

  const PixelContainer* GetPixelContainer() const
    { return m_Buffer.GetPointer(); }

  /** Set the container to use. Note that this does not cause the
   * DataObject to be modified. */
  void SetPixelContainer( PixelContainer *container );

  /** Graft the data and information from one image to another. This
   * is a convenience method to setup a second image with all the meta
   * information of another image and use the same pixel
   * container. Note that this method is different than just using two
   * SmartPointers to the same image since separate DataObjects are
   * still maintained. This method is similar to
   * ImageSource::GraftOutput(). The implementation in ImageBase
   * simply calls CopyInformation() and copies the region ivars.
   * The implementation here refers to the superclass' implementation
   * and then copies over the pixel container. */
  virtual void Graft(const DataObject *data);

  /** Return the Pixel Accessor object */
  AccessorType GetPixelAccessor( void )
    { return AccessorType( m_Buffer->GetBufferPointer() ); }

  /** Return the Pixel Accesor object */
  const AccessorType GetPixelAccessor( void ) const
    { return AccessorType( m_Buffer->GetBufferPointer() ); }

  /** Return the NeighborhoodAccessor functor */
  NeighborhoodAccessorFunctorType GetNeighborhoodAccessor()
    { return NeighborhoodAccessorFunctorType(); }

  /** Return the NeighborhoodAccessor functor */
  const NeighborhoodAccessorFunctorType GetNeighborhoodAccessor() const
    { return NeighborhoodAccessorFunctorType(); }

protected:
  SingleBitBinaryImage();
  void PrintSelf(std::ostream& os, Indent indent) const;
  virtual ~SingleBitBinaryImage() {};

private:
  SingleBitBinaryImage(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Memory for the current buffer. */
  PixelContainerPointer m_Buffer;

};

} // end namespace itk

// Define instantiation macro for this template.
#define ITK_TEMPLATE_SingleBitBinaryImage(_, EXPORT, x, y) namespace itk { \
  _(2(class EXPORT SingleBitBinaryImage< ITK_TEMPLATE_2 x >)) \
  namespace Templates { typedef SingleBitBinaryImage< ITK_TEMPLATE_2 x > SingleBitBinaryImage##y; } \
  }

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkSingleBitBinaryImage+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkSingleBitBinaryImage.txx"
#endif

#endif
