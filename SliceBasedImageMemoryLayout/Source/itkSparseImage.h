/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSparseImage.h,v $
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSparseImage_h
#define __itkSparseImage_h

#include "itkImageBase.h"
#include "itkImageRegion.h"
#include "itkSparseImageContainer.h"
#include "itkSparseImagePixelAccessor.h"
#include "itkSparseImagePixelAccessorFunctor.h"
#include "itkSparseImageNeighborhoodAccessorFunctor.h"
#include "itkNeighborhoodAccessorFunctor.h"
#include "itkPoint.h"
#include "itkContinuousIndex.h"
#include "itkWeakPointer.h"

namespace itk
{

/** \class SparseImage
 *  \brief An n-dimensional image with a sparse memory model.
 *
 * The elements for a normal itk::Image are stored in a single, contiguous
 * 1-D array. The elements for this image are stored in a hash table,
 * catering for very large images with a small number of relevant pixels.
 *
 * The class is templated over the pixel type and image dimension.
 *
 * \ingroup ImageObjects
 */
template <class TPixel, unsigned int VImageDimension>
class ITK_EXPORT SparseImage :
    public ImageBase< VImageDimension >
{
public:
  /** Standard class typedefs */
  typedef SparseImage                  Self;
  typedef ImageBase< VImageDimension > Superclass;
  typedef SmartPointer<Self>           Pointer;
  typedef SmartPointer<const Self>     ConstPointer;
  typedef WeakPointer<const Self>      ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(SparseImage, ImageBase);

  /** Pixel typedef support. */
  typedef TPixel PixelType;
  
  /** This is the actual pixel type contained in the buffer. */
  typedef TPixel InternalPixelType;

  /** Typedef alias for PixelType */
  typedef TPixel ValueType;

  typedef InternalPixelType IOPixelType;

  /** Dimension of the image.  This constant is used by functions that are
   * templated over image type (as opposed to being templated over pixel type
   * and dimension) when they need compile time access to the dimension of
   * the image. */
  itkStaticConstMacro( ImageDimension, unsigned int, VImageDimension );

  /** Container used to store pixels in the image. */
  typedef SparseImageContainer<unsigned long, InternalPixelType> PixelContainer;

  /** Index typedef support. An index is used to access pixel values. */
  typedef typename Superclass::IndexType IndexType;

  /** Offset typedef support. An offset is used to access pixel values. */
  typedef typename Superclass::OffsetType OffsetType;

  /** Size typedef support. A size is used to define region bounds. */
  typedef typename Superclass::SizeType SizeType;

  /** Direction typedef support. A matrix of direction cosines. */
  typedef typename Superclass::DirectionType DirectionType;

  /** Region typedef support. A region is used to specify a subset of an image. */
  typedef typename Superclass::RegionType RegionType;

  /** Spacing typedef support.  Spacing holds the size of a pixel.  The
   * spacing is the geometric distance between image samples. */
  typedef typename Superclass::SpacingType SpacingType;

  /** Origin typedef support.  The origin is the geometric coordinates
   * of the index (0,0). */
  typedef typename Superclass::PointType PointType;

  /** A pointer to the pixel container. */
  typedef typename PixelContainer::Pointer PixelContainerPointer;
  typedef typename PixelContainer::ConstPointer PixelContainerConstPointer;

  /** Offset typedef (relative position between indices) */
  typedef typename Superclass::OffsetValueType OffsetValueType;

  /** Accessor type that convert data between internal and external
   *  representations. */
  typedef SparseImagePixelAccessor<
    InternalPixelType, typename PixelContainer::PixelMapType > AccessorType;

  /** Tyepdef for the functor used to access pixels.*/
  typedef SparseImagePixelAccessorFunctor< Self > AccessorFunctorType;

  /** Tyepdef for the functor used to access a neighborhood of pixel pointers.*/
  typedef SparseImageNeighborhoodAccessorFunctor< Self >
    NeighborhoodAccessorFunctorType;

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
    }

  void SetRegions(SizeType size)
    {
    RegionType region; region.SetSize(size);
    this->SetLargestPossibleRegion(region);
    this->SetBufferedRegion(region);
    this->SetRequestedRegion(region);
    }

  /** Buffered region has no meaning for sparse images.
   *  This method does nothing.
   * \sa ImageRegion, SetLargestPossibleRegion(), SetRequestedRegion() */
  virtual void SetBufferedRegion(const RegionType &region) { }

  /** Buffered region has no meaning for sparse images.
   *  This method always returns the largest possible region.
   * \sa ImageRegion, SetLargestPossibleRegion(), SetRequestedRegion() */
  virtual const RegionType& GetBufferedRegion() const
  { return this->GetLargestPossibleRegion(); }

  /** Restore the data object to its initial state. This means releasing
   * memory. */
  virtual void Initialize();

  OffsetValueType ComputeOffset(const IndexType &ind) const
  {
    // need to add bounds checking for the region/buffer?
    OffsetValueType offset=0;
    const OffsetValueType *offsetTable = this->GetOffsetTable(); 
  
    // data is arranged as [][][][slice][row][col]
    // with Index[0] = col, Index[1] = row, Index[2] = slice
    for (int i=ImageDimension-1; i > 0; i--)
      {
      offset += ind[i]*offsetTable[i];
      }
    offset += ind[0];

    return offset;
  }

  /** Fill the image buffer with a value.  Be sure to call Allocate()
   * first. */
  void FillBuffer(const PixelType& value);

  /** \brief Set a pixel value.
   *
   * Allocate() needs to have been called first -- for efficiency,
   * this function does not check that the image has actually been
   * allocated yet. */
   void SetPixel( const IndexType &index, const PixelType& value )
    {
    typename PixelContainer::PixelMapType *map = m_Container->GetPixelMap();
    OffsetValueType offset = this->ComputeOffset(index);
    map->operator[](offset) = value;
    }

  /** \brief Get a pixel (read only version).
   *
   * For efficiency, this function does not check that the
   * image has actually been allocated yet. Note that the method returns a 
   * pixel on the stack. */
  const PixelType GetPixel(const IndexType &index) const
    {
    typename PixelContainer::PixelMapType *map = m_Container->GetPixelMap();
    OffsetValueType offset = this->ComputeOffset(index);
    typename PixelContainer::PixelMapType::const_iterator it = map->find( offset );
    if ( it == map->end() )
      {
      return m_FillBufferValue;
      }
    else
      {
      return map->operator[](offset);
      }
    }

  /** \brief Get a reference to a pixel (e.g. for editing).
   *
   * For efficiency, this function does not check that the
   * image has actually been allocated yet. */
  PixelType GetPixel(const IndexType &index )
    {
    typename PixelContainer::PixelMapType *map = m_Container->GetPixelMap();
    OffsetValueType offset = this->ComputeOffset(index);
    typename PixelContainer::PixelMapType::const_iterator it = map->find( offset );
    if ( it == map->end() )
      {
      return m_FillBufferValue;
      }
    else
      {
      return map->operator[](offset);
      }
    }

  /** \brief Access a pixel. This version cannot be an lvalue because the pixel
   * is converted on the fly to a VariableLengthVector.
   *
   * For efficiency, this function does not check that the
   * image has actually been allocated yet. */
  PixelType operator[](const IndexType &index)
     { return this->GetPixel(index); }

  /** \brief Access a pixel.
   *
   * For efficiency, this function does not check that the
   * image has actually been allocated yet. */
  PixelType operator[](const IndexType &index) const
     { return this->GetPixel(index); }

  /** Sparse images do not have buffer. This method always returns 0 */
  InternalPixelType * GetBufferPointer()
    { return 0; }
  const InternalPixelType * GetBufferPointer() const
    { return 0; }

  /** Return a pointer to the container. */
  PixelContainer* GetPixelContainer()
    { return m_Container.GetPointer(); }

  /** Return a pointer to the container. */
  const PixelContainer* GetPixelContainer() const
    { return m_Container.GetPointer(); }

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
    {
      return AccessorType(
        m_Container->GetPixelMap(),
        m_FillBufferValue
      );
    }

  /** Return the Pixel Accesor object */
  const AccessorType GetPixelAccessor( void ) const
    {
      return AccessorType(
        m_Container->GetPixelMap(),
        m_FillBufferValue
      );
    }

  /** Return the NeighborhoodAccessor functor */
  NeighborhoodAccessorFunctorType GetNeighborhoodAccessor() 
    {
      return NeighborhoodAccessorFunctorType(
        m_Container->GetPixelMap(),
        m_FillBufferValue
       );
    }

  /** Return the NeighborhoodAccessor functor */
  const NeighborhoodAccessorFunctorType GetNeighborhoodAccessor() const
    {
      return NeighborhoodAccessorFunctorType(
        m_Container->GetPixelMap(),
        m_FillBufferValue
       );
    }

protected:
  SparseImage();
  void PrintSelf( std::ostream& os, Indent indent ) const;
  virtual ~SparseImage() {};
  
private:
  SparseImage( const Self & ); // purposely not implementated
  void operator=(const Self&); //purposely not implemented

  /** Memory for the map containing the pixel data. */
  PixelContainerPointer m_Container;
  PixelType m_FillBufferValue;
}; 


} // end namespace itk

// Define instantiation macro for this template.
#define ITK_TEMPLATE_SparseImage(_, EXPORT, x, y) namespace itk { \
  _(2(class EXPORT SparseImage< ITK_TEMPLATE_2 x >)) \
  namespace Templates { typedef SparseImage< ITK_TEMPLATE_2 x > SparseImage##y; } \
  }

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkSparseImage+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkSparseImage.txx"
#endif

#endif
