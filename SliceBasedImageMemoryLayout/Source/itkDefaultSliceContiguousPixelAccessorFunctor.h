/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDefaultSliceContiguousPixelAccessorFunctor.h,v $
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkDefaultSliceContiguousPixelAccessorFunctor_h
#define __itkDefaultSliceContiguousPixelAccessorFunctor_h

namespace itk
{
/** \class DefaultSliceContiguousPixelAccessorFunctor
 * \brief This class provides a common API for pixel accessors for Image and 
 * SliceContiguousImage. (between the DefaultVectorPixelAccessor and DefaultPixelAccessor).
 * 
 * The pixel accessor is set with the SetPixelAccessor method. This accessor is 
 * meant to be used only for SliceContiguousImage and not for Image. Prior to use,
 * this slices must be set. TODO:
 *
 * \sa DefaultSliceContiguousPixelAccessor
 * \sa DefaultPixelAccessor
 * \sa DefaultPixelAccessorFunctor
 *
 * \ingroup ImageAdaptors
 */
template <class TImageType >
class ITK_EXPORT DefaultSliceContiguousPixelAccessorFunctor
{
public:

  typedef TImageType                                   ImageType;
  typedef typename ImageType::InternalPixelType        InternalPixelType;
  typedef typename ImageType::PixelType                ExternalPixelType;
  typedef typename ImageType::AccessorType             PixelAccessorType;
  typedef unsigned int                                 VectorLengthType;
  
  /** Set the PixelAccessor. This is set at construction time by the image iterators.
   * The type PixelAccessorType is obtained from the ImageType over which the iterators
   * are templated.
   * */
  inline void SetPixelAccessor( PixelAccessorType& accessor ) 
    {
    m_PixelAccessor = accessor;
    }

  /** Set the pointer index to the start of the buffer. */
  inline void SetBegin( const InternalPixelType * begin ) 
    { this->m_Begin = const_cast< InternalPixelType * >( begin ); }
  
  /** Set output using the value in input */
  inline void Set( InternalPixelType & output, const ExternalPixelType &input ) const
    {
    m_PixelAccessor.Set( output, input, (&output)-m_Begin );
    }

  /** Get the value from input */
  inline ExternalPixelType Get( const InternalPixelType &input ) const
    {
    return m_PixelAccessor.Get( input, &input  - m_Begin );
    }

private:
  PixelAccessorType m_PixelAccessor; // The pixel accessor
  InternalPixelType *m_Begin; // Begin of the buffer
};

}

#endif    
