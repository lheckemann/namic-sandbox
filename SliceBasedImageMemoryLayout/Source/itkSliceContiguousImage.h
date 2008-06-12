/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSliceContiguousImage.h,v $
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSliceContiguousImage_h
#define __itkSliceContiguousImage_h

#include "itkImageBase.h"
#include "itkImageRegion.h"
#include "itkImportSliceContiguousImageContainer.h"
#include "itkDefaultSliceContiguousPixelAccessor.h"
#include "itkDefaultSliceContiguousPixelAccessorFunctor.h"
//#include "itkSliceContiguousImageNeighborhoodAccessorFunctor.h" // TODO:
#include "itkNeighborhoodAccessorFunctor.h"
#include "itkPoint.h"
#include "itkContinuousIndex.h"
#include "itkWeakPointer.h"

namespace itk
{

/** \class SliceContiguousImage
 *  \brief Templated n-dimensional image class with slice contiguous memory model.
 * 
 * TODO:
 *
 * \ingroup ImageObjects 
 */
template <class TPixel, unsigned int VImageDimension=3 >
class ITK_EXPORT SliceContiguousImage :
    public ImageBase< VImageDimension >
{
public:
  /** Standard class typedefs */
  typedef SliceContiguousImage         Self;
  typedef ImageBase< VImageDimension > Superclass;
  typedef SmartPointer<Self>           Pointer;
  typedef SmartPointer<const Self>     ConstPointer;
  typedef WeakPointer<const Self>      ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(SliceContiguousImage, ImageBase);

  /** Pixel typedef support. */
  typedef TPixel PixelType;
  
  /** This is the actual pixel type contained in the buffer. */
  typedef TPixel InternalPixelType;

  /** Typedef alias for PixelType */
  typedef TPixel ValueType;

  typedef InternalPixelType  IOPixelType;

  /** Dimension of the image.  This constant is used by functions that are
   * templated over image type (as opposed to being templated over pixel type
   * and dimension) when they need compile time access to the dimension of
   * the image. */
  itkStaticConstMacro(ImageDimension, unsigned int, VImageDimension);

  /** Container used to store pixels in the image. */
  typedef ImportSliceContiguousImageContainer<unsigned long, InternalPixelType> PixelContainer;

  /** Index typedef support. An index is used to access pixel values. */
  typedef typename Superclass::IndexType  IndexType;

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
  typedef typename PixelContainer::Pointer PixelContainerPointer;
  typedef typename PixelContainer::ConstPointer PixelContainerConstPointer;

  /** Offset typedef (relative position between indices) */
  typedef typename Superclass::OffsetValueType OffsetValueType;

  /** Accessor type that convert data between internal and external
   *  representations.  */
  typedef DefaultSliceContiguousPixelAccessor< InternalPixelType, SizeType > AccessorType;

  /** Functor to provide a common API between DefaultPixelAccessor and
   * DefaultVectorPixelAccessor */
  typedef DefaultSliceContiguousPixelAccessorFunctor< Self > AccessorFunctorType;

  /** Tyepdef for the functor used to access a neighborhood of pixel pointers.*/
  typedef NeighborhoodAccessorFunctor< Self >  NeighborhoodAccessorFunctorType;
  // TODO:
  //typedef SliceContiguousImageNeighborhoodAccessorFunctor<
  //                        Self >              NeighborhoodAccessorFunctorType;

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
    };

  /** Restore the data object to its initial state. This means releasing
   * memory. */
  virtual void Initialize();

  OffsetValueType ComputeOffset(const IndexType &ind) const
  {
    // need to add bounds checking for the region/buffer?
    OffsetValueType offset=0;
    const IndexType &bufferedRegionIndex = this->GetBufferedRegion().GetIndex();
    const OffsetValueType *offsetTable = this->GetOffsetTable(); 
  
    // data is arranged as [][][][slice][row][col]
    // with Index[0] = col, Index[1] = row, Index[2] = slice
    for (int i=VImageDimension-1; i > 0; i--)
      {
      offset += (ind[i] - bufferedRegionIndex[i])*offsetTable[i];
      }
    offset += (ind[0] - bufferedRegionIndex[0]);

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
// TODO:
//     OffsetValueType offset = m_VectorLength * this->ComputeOffset(index);
//     for( VectorLengthType i = 0; i < m_VectorLength; i++ )
//       {
//       (*m_Buffer)[offset + i] = value[i];
//       }
    }

  /** \brief Get a pixel (read only version).
   *
   * For efficiency, this function does not check that the
   * image has actually been allocated yet. Note that the method returns a 
   * pixel on the stack. */
  const PixelType GetPixel(const IndexType &index) const
    {
// TODO:
//     OffsetValueType offset = m_VectorLength * this->ComputeOffset(index);
//     PixelType p( &((*m_Buffer)[offset]), m_VectorLength ); 
//     return p;
    }

  /** \brief Get a reference to a pixel (e.g. for editing).
   *
   * For efficiency, this function does not check that the
   * image has actually been allocated yet. */
  PixelType GetPixel(const IndexType &index )
    {
// TODO:
//     OffsetValueType offset = m_VectorLength * this->ComputeOffset(index);
//     PixelType p( &((*m_Buffer)[offset]), m_VectorLength ); 
//     return p;
    }

  /** \brief Access a pixel. This version cannot be an lvalue because the pixel
   * is converted on the fly to a VariableLengthVector.
   *
   * For efficiency, this function does not check that the
   * image has actually been allocated yet. */
  PixelType operator[](const IndexType &index)
     { return this->GetPixel(index); }

  /** \brief Access a pixel. This version can only be an rvalue because the
   * pixel is converted on the fly to a VariableLengthVector.
   *
   * For efficiency, this function does not check that the
   * image has actually been allocated yet. */
  PixelType operator[](const IndexType &index) const
     { return this->GetPixel(index); }

  /** Return a pointer to the beginning of the buffer.  This is used by
   * the image iterator class. */
  InternalPixelType * GetBufferPointer()
    { return 0; }
  const InternalPixelType *GetBufferPointer() const
    { return 0; }

  /** Return a pointer to the container. */
  PixelContainer* GetPixelContainer()
    { return m_Buffer.GetPointer(); }

  /** Return a pointer to the container. */
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
    { 
      return AccessorType( 
        m_Buffer->GetSlices(),
        this->GetLargestPossibleRegion().GetSize()
      );
    }

  /** Return the Pixel Accesor object */
  const AccessorType GetPixelAccessor( void ) const
    { 
      return AccessorType( 
        m_Buffer->GetSlices(),
        this->GetLargestPossibleRegion().GetSize()
      );
    }

  /** Return the NeighborhoodAccessor functor */
  NeighborhoodAccessorFunctorType GetNeighborhoodAccessor() 
    { return NeighborhoodAccessorFunctorType( /* TODO: */ ); }

  /** Return the NeighborhoodAccessor functor */
  const NeighborhoodAccessorFunctorType GetNeighborhoodAccessor() const
    { return NeighborhoodAccessorFunctorType( /* TODO: */ ); }

  /** \brief Get the continuous index from a physical point
   *
   * Returns true if the resulting index is within the image, false otherwise.
   * \sa Transform */
  template<class TCoordRep>
  bool TransformPhysicalPointToContinuousIndex(
              const Point<TCoordRep, VImageDimension>& point,
              ContinuousIndex<TCoordRep, VImageDimension>& index   ) const
    {
    // Update the output index
    for (unsigned int i = 0 ; i < VImageDimension ; i++)
      {
      index[i] = static_cast<TCoordRep>( (point[i]- this->m_Origin[i]) / this->m_Spacing[i] );
      }

    // Now, check to see if the index is within allowed bounds
    const bool isInside =
      this->GetLargestPossibleRegion().IsInside( index );

    return isInside;
    }

  /** Get the index (discrete) from a physical point.
   * Floating point index results are truncated to integers.
   * Returns true if the resulting index is within the image, false otherwise
   * \sa Transform */
  template<class TCoordRep>
  bool TransformPhysicalPointToIndex(
            const Point<TCoordRep, VImageDimension>& point,
            IndexType & index                                ) const
    {
    typedef typename IndexType::IndexValueType IndexValueType;

    // Update the output index
    for (unsigned int i = 0 ; i < VImageDimension ; i++)
      {
      index[i] = static_cast<IndexValueType>( (point[i]- this->m_Origin[i]) / this->m_Spacing[i] );
      }

    // Now, check to see if the index is within allowed bounds
    const bool isInside =
      this->GetLargestPossibleRegion().IsInside( index );

    return isInside;
    }

  /** Get a physical point (in the space which
   * the origin and spacing infomation comes from)
   * from a continuous index (in the index space)
   * \sa Transform */
  template<class TCoordRep>
  void TransformContinuousIndexToPhysicalPoint(
            const ContinuousIndex<TCoordRep, VImageDimension>& index,
            Point<TCoordRep, VImageDimension>& point        ) const
    {
    for (unsigned int i = 0 ; i < VImageDimension ; i++)
      {
      point[i] = static_cast<TCoordRep>( this->m_Spacing[i] * index[i] + this->m_Origin[i] );
      }
    }

  /** Get a physical point (in the space which
   * the origin and spacing infomation comes from)
   * from a discrete index (in the index space)
   *
   * \sa Transform */
  template<class TCoordRep>
  void TransformIndexToPhysicalPoint(
                      const IndexType & index,
                      Point<TCoordRep, VImageDimension>& point ) const
    {
    for (unsigned int i = 0 ; i < VImageDimension ; i++)
      {
      point[i] = static_cast<TCoordRep>( this->m_Spacing[i] *
        static_cast<double>( index[i] ) + this->m_Origin[i] );
      }
    }

protected:
  SliceContiguousImage();
  void PrintSelf( std::ostream& os, Indent indent ) const;
  virtual ~SliceContiguousImage() {};
  
private:
  SliceContiguousImage( const Self & ); // purposely not implementated
  void operator=(const Self&); //purposely not implemented

  /** Memory for the current buffer. */
  PixelContainerPointer m_Buffer;

}; 


} // end namespace itk

// Define instantiation macro for this template.
#define ITK_TEMPLATE_SliceContiguousImage(_, EXPORT, x, y) namespace itk { \
  _(2(class EXPORT SliceContiguousImage< ITK_TEMPLATE_2 x >)) \
  namespace Templates { typedef SliceContiguousImage< ITK_TEMPLATE_2 x > SliceContiguousImage##y; } \
  }

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkSliceContiguousImage+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkSliceContiguousImage.txx"
#endif

#endif
