/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSingleBitBinaryImageNeighborhoodAccessorFunctor.h,v $
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSingleBitBinaryImageNeighborhoodAccessorFunctor_h
#define __itkSingleBitBinaryImageNeighborhoodAccessorFunctor_h

#include "itkImageBoundaryCondition.h"
#include "itkNeighborhood.h"
#include "itkImageBase.h"

namespace itk
{

/** \class SingleBitBinaryImageNeighborhoodAccessorFunctor
 * \brief Provides accessor interfaces to Access pixels and is meant to be
 * used on pointers to pixels held by the Neighborhood class.
 *
 * A typical user should not need to use this class. The class is internally
 * used by the neighborhood iterators.
 *
 */
template< class TImage >
class SingleBitBinaryImageNeighborhoodAccessorFunctor
{
public:
  typedef TImage                                ImageType;
  typedef typename ImageType::PixelType         PixelType;
  typedef typename ImageType::InternalPixelType InternalPixelType;
  typedef unsigned int                          VectorLengthType;
  typedef typename ImageType::OffsetType        OffsetType;

  typedef Neighborhood< InternalPixelType *,
          ::itk::GetImageDimension< TImage >::ImageDimension> NeighborhoodType;

  typedef ImageBoundaryCondition< ImageType > const 
                          *ImageBoundaryConditionConstPointerType;

  SingleBitBinaryImageNeighborhoodAccessorFunctor() {};

  /** Set the pointer index to the start of the buffer.
   * This must be set by the iterators to the starting location of the buffer.
   * Typically a neighborhood iterator iterating on a neighborhood of an Image,
   * say \c image will set this in its constructor. For instance:
   * 
   * \code
   * ConstNeighborhoodIterator( radius, image, )
   *   {
   *   ...
   *   m_NeighborhoodAccessorFunctor.SetBegin( image->GetBufferPointer() );
   *   }
   * \endcode
   */
  inline void SetBegin( const InternalPixelType * begin )
    { this->m_Begin = const_cast< InternalPixelType * >( begin ); }

  /** Method to dereference a pixel pointer. This is used from the 
   * ConstNeighborhoodIterator as the equivalent operation to (*it).
   * This method should be preferred over the former (*it) notation. 
   * The reason is that dereferencing a pointer to a location of 
   * VectorImage pixel involves a different operation that simply
   * dereferencing the pointer. Here a PixelType (array of InternalPixelType s)
   * is created on the stack and returned.  */
  inline PixelType Get( const InternalPixelType *pixelPointer ) const
    {
    unsigned long offset = pixelPointer  - m_Begin;
    unsigned long groupOffset = offset / 32;
    unsigned long bitOffset = offset % 32;
    InternalPixelType bitMask = 0x1 << bitOffset;
    return (m_Begin[groupOffset] & bitMask) == bitMask;
    }

  /** Method to set the pixel value at a certain pixel pointer */
  inline void Set( InternalPixelType* &pixelPointer, const PixelType &p ) const
    {
    unsigned long offset = pixelPointer - m_Begin;
    unsigned long groupOffset = offset / 32;
    unsigned long bitOffset = offset % 32;
    InternalPixelType bitMask = ( 0x1 << bitOffset );
    if ( p )
      {
      m_Begin[groupOffset] |= bitMask;
      }
    else
      {
      m_Begin[groupOffset] &= ~bitMask;
      }
    }

  inline PixelType BoundaryCondition(
      const OffsetType& point_index,
      const OffsetType &boundary_offset,
      const NeighborhoodType *data,
      const ImageBoundaryConditionConstPointerType boundaryCondition) const
    {
    return boundaryCondition->operator()(point_index, boundary_offset, data, *this);
    }

private:
  InternalPixelType *m_Begin;  // Begin of the buffer
};

} // end namespace itk
#endif
