/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSparseImagePixelAccessor.h,v $
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSparseImagePixelAccessor_h
#define __itkSparseImagePixelAccessor_h

#include "itkMacro.h"

namespace itk
{

/** \class SparseImagePixelAccessor
 * \brief Give access to partial aspects of a type
 *
 * SparseImagePixelAccessor is specifically meant to provide SparseImage
 * with the same \c DefaultPixelAccessor interface that
 * DefaultPixelAccessor provides to Image.
 *
 * SparseImagePixelAccessor is templated over an internal type and an
 * external type representation. This class encapsulates a
 * customized convertion between the internal and external
 * type representations.
 *
 * \sa SparseImagePixelAccessor
 * \sa DefaultPixelAccessor
 * \sa DefaultPixelAccessorFunctor
 *
 * \ingroup ImageAdaptors
 */
template <class TType, class TPixelMapType>
class ITK_EXPORT SparseImagePixelAccessor
{
public:

 /** External typedef. It defines the external aspect
   * that this class will exhibit. */
  typedef TType ExternalType;

  /** Internal typedef. It defines the internal real
   * representation of data. */
  typedef TType InternalType;

  /** Typedef for pixel map. */
  typedef TPixelMapType PixelMapType;

  /** Set output using the value in input */
  inline void Set(InternalType & output, const ExternalType & input, const unsigned long offset ) const
    {
    m_PixelMap->operator[](offset) = input;
    }

  /** Get the value from input */
  inline ExternalType Get( const InternalType & begin, const unsigned long offset ) const
    {
    typename PixelMapType::const_iterator it = m_PixelMap->find( offset );
    if ( it == m_PixelMap->end() )
      {
      return m_FillBufferValue;
      }
    else
      {
      return m_PixelMap->operator[](offset);
      }
    }

  SparseImagePixelAccessor() {}

   /** Constructor to initialize slices and image size at construction time */
   SparseImagePixelAccessor( PixelMapType* pixelMap, TType fillBufferValue )
     {
     m_PixelMap = pixelMap;
     m_FillBufferValue = fillBufferValue;
     }

  virtual ~SparseImagePixelAccessor() {};

private:
  PixelMapType* m_PixelMap;
  InternalType m_FillBufferValue;
};

} // end namespace itk


#endif
