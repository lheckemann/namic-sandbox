/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSingleBitBinaryImagePixelAccessor.h,v $
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSingleBitBinaryImagePixelAccessor_h
#define __itkSingleBitBinaryImagePixelAccessor_h

#include "itkMacro.h"

namespace itk
{

/** \class SingleBitBinaryImagePixelAccessor
 * \brief Give access to partial aspects of a type
 *
 * SingleBitBinaryImagePixelAccessor is specifically meant to provide
 * SingleBitBinaryImage with the same \c DefaultPixelAccessor interface that
 * DefaultPixelAccessor provides to Image.
 *
 * SingleBitBinaryImagePixelAccessor is templated over an internal type and an
 * external type representation. This class encapsulates a
 * customized convertion between the internal and external
 * type representations.
 *
 * \sa SingleBitBinaryImagePixelAccessor
 * \sa DefaultPixelAccessor
 * \sa DefaultPixelAccessorFunctor
 *
 * \ingroup ImageAdaptors
 */
template <class TExternalType, class TInternalType>
class ITK_EXPORT SingleBitBinaryImagePixelAccessor
{
public:

 /** External typedef. It defines the external aspect
   * that this class will exhibit. */
  typedef TExternalType ExternalType;

  /** Internal typedef. It defines the internal real
   * representation of data. */
  typedef TInternalType InternalType;

  /** Set output using the value in input */
  inline void Set(InternalType & output, const ExternalType & input, const unsigned long offset ) const
    {
    unsigned long groupOffset = offset / 32;
    unsigned long bitOffset = offset % 32;
    InternalType bitMask = ( 0x1 << bitOffset );
    if ( input )
      {
      m_Begin[groupOffset] |= bitMask;
      }
    else
      {
      m_Begin[groupOffset] &= ~bitMask;
      }
    }

  /** Get the value from input */
  inline ExternalType Get( const InternalType & input, const unsigned long offset ) const
    {
    unsigned long groupOffset = offset / 32;
    unsigned long bitOffset = offset % 32;
    InternalType bitMask = 0x1 << bitOffset;
    return (m_Begin[groupOffset] & bitMask) == bitMask;
    }

  SingleBitBinaryImagePixelAccessor() {}

   /** Constructor to initialize pixel buffer at construction time */
   SingleBitBinaryImagePixelAccessor( InternalType* begin )
     {
     m_Begin = begin;
     }

  virtual ~SingleBitBinaryImagePixelAccessor() {};

private:
  InternalType* m_Begin;
};

} // end namespace itk


#endif
