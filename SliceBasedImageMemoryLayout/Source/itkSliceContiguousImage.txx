/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSliceContiguousImage.txx,v $
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkSliceContiguousImage_txx
#define _itkSliceContiguousImage_txx

#include "itkSliceContiguousImage.h"
#include "itkProcessObject.h"

namespace itk
{

/**
 *
 */
template<class TPixel, unsigned int VImageDimension>
SliceContiguousImage<TPixel, VImageDimension>
::SliceContiguousImage()
{
  // TODO: m_Buffer = PixelContainer::New();
  m_Buffer = 0;
}


//----------------------------------------------------------------------------
template<class TPixel, unsigned int VImageDimension>
void
SliceContiguousImage<TPixel, VImageDimension>
::Allocate()
{
  unsigned long num;
  this->ComputeOffsetTable();
  num = this->GetOffsetTable()[VImageDimension];
  //m_Buffer->Reserve(num);
}

template<class TPixel, unsigned int VImageDimension>
void 
SliceContiguousImage<TPixel, VImageDimension>
::Initialize()
{
  //
  // We don't modify ourselves because the "ReleaseData" methods depend upon
  // no modification when initialized.
  //

  // Call the superclass which should initialize the BufferedRegion ivar.
  Superclass::Initialize();

  // Replace the handle to the buffer. This is the safest thing to do,
  // since the same container can be shared by multiple images (e.g.
  // Grafted outputs and in place filters).
  // TODO: m_Buffer = PixelContainer::New();
}


template<class TPixel, unsigned int VImageDimension>
void 
SliceContiguousImage<TPixel, VImageDimension>
::FillBuffer(const PixelType& value)
{
  const unsigned long numberOfPixels =
    this->GetBufferedRegion().GetNumberOfPixels();

  unsigned long ctr = 0;
  for(unsigned int i=0; i<numberOfPixels; i++) 
    {
// TODO:      
//     for( VectorLengthType j=0; j<m_VectorLength; j++ )
//       {
//       (*m_Buffer)[ctr++] = value[j];
//       }
    }
}

template<class TPixel, unsigned int VImageDimension>
void
SliceContiguousImage<TPixel, VImageDimension>
::SetPixelContainer(PixelContainer *container)
{
   if (m_Buffer != container)
     {
     m_Buffer = container;
     this->Modified();
     }
}
    
//----------------------------------------------------------------------------
template<class TPixel, unsigned int VImageDimension>
void 
SliceContiguousImage<TPixel, VImageDimension>
::Graft(const DataObject *data)
{
  // call the superclass' implementation
  Superclass::Graft( data );

  if ( data )
    {
    // Attempt to cast data to an Image
    const Self *imgData;

    try
      {
      imgData = dynamic_cast< const Self *>( data );
      }
    catch( ... )
      {
      return;
      }

    // Copy from VectorImage< TPixel, dim >
    if ( imgData )
      {
      // Now copy anything remaining that is needed
      this->SetPixelContainer( const_cast< PixelContainer *>
                                    (imgData->GetPixelContainer()) );
      }
    else 
      {
      // pointer could not be cast back down
      itkExceptionMacro( << "itk::SliceContiguousImage::Graft() cannot cast "
                         << typeid(data).name() << " to "
                         << typeid(const Self *).name() );
      }
    }
}

/**
 *
 */
template<class TPixel, unsigned int VImageDimension>
void 
SliceContiguousImage<TPixel, VImageDimension>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "PixelContainer: " << std::endl;
  // TODO: m_Buffer->Print(os, indent.GetNextIndent());

// m_Origin and m_Spacing are printed in the Superclass
}
} // end namespace itk

#endif
