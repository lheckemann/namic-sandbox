/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDefaultSliceContiguousPixelAccessor.h,v $
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkDefaultSliceContiguousPixelAccessor_h
#define __itkDefaultSliceContiguousPixelAccessor_h

#include "itkMacro.h"

namespace itk
{

/** \class DefaultSliceContiguousPixelAccessor
 * \brief Give access to partial aspects of a type
 *
 * DefaultSliceContiguousPixelAccessor is specifically meant to provide SliceContiguousImage
 * with the same \c DefaultPixelAccessor interface that
 * DefaultPixelAccessor provides to Image.
 *
 * DefaultSliceContiguousPixelAccessor is templated over an internal type and an
 * external type representation. This class encapsulates a
 * customized convertion between the internal and external
 * type representations.
 *
 * \sa DefaultSliceContiguousPixelAccessor
 * \sa DefaultPixelAccessor
 * \sa DefaultPixelAccessorFunctor
 *
 * \ingroup ImageAdaptors
 */
template <class TType, class TSize >
class ITK_EXPORT DefaultSliceContiguousPixelAccessor
{
public:

 /** External typedef. It defines the external aspect
   * that this class will exhibit. */
  typedef TType ExternalType ;

  /** Internal typedef. It defines the internal real
   * representation of data. */
  typedef TType InternalType ;

  /** Typedef for slices array. */
  typedef std::vector< TType * > SliceArrayType;

  /** Typedef for image size. */
  typedef TSize SizeType;

  /** Set output using the value in input */
  inline void Set(InternalType & output, const ExternalType & input, const unsigned long offset ) const
    {
      const unsigned long slice = offset / m_SizeOfSlice;
      const unsigned long sliceOffset = offset % m_SizeOfSlice;
      InternalType *truePixel = (m_Slices->operator[](slice) + sliceOffset);
      *truePixel = input;
    }

  /** Get the value from input */
  inline ExternalType Get( const InternalType & begin, const unsigned long offset ) const
    {
      const unsigned long slice = offset / m_SizeOfSlice;
      const unsigned long sliceOffset = offset % m_SizeOfSlice;
      return *(m_Slices->operator[](slice) + sliceOffset);
    }

  DefaultSliceContiguousPixelAccessor() {}

   /** Constructor to initialize slices and image size at construction time */
   DefaultSliceContiguousPixelAccessor( SliceArrayType* slices, SizeType size )
     {
     m_Slices = slices;
     m_Size = size;
     m_SizeOfSlice = size[0] * size[1]; // Pre-compute for speed
     }

  virtual ~DefaultSliceContiguousPixelAccessor() {};

private:
  SliceArrayType* m_Slices;
  SizeType m_Size;
  unsigned long m_SizeOfSlice;
};

} // end namespace itk


#endif
