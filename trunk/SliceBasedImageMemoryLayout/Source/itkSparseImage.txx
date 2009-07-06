/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSparseImage.txx,v $
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
#ifndef _itkSparseImage_txx
#define _itkSparseImage_txx

#include "itkSparseImage.h"
#include "itkProcessObject.h"

namespace itk
{

template<class TPixel, unsigned int VImageDimension>
SparseImage<TPixel, VImageDimension>
::SparseImage()
{
  m_Container = PixelContainer::New();
}


template<class TPixel, unsigned int VImageDimension>
void
SparseImage<TPixel, VImageDimension>
::Allocate()
{
  this->ComputeOffsetTable();
  m_Container->Reserve( );
}


template<class TPixel, unsigned int VImageDimension>
void
SparseImage<TPixel, VImageDimension>
::Initialize()
{
  //
  // We don't modify ourselves because the "ReleaseData" methods depend upon
  // no modification when initialized.
  //

  // Call the superclass which should initialize the BufferedRegion ivar.
  Superclass::Initialize();

  // Replace the handle to the container. This is the safest thing to do,
  // since the same container can be shared by multiple images (e.g.
  // Grafted outputs and in place filters).
  m_Container = PixelContainer::New();
}


template<class TPixel, unsigned int VImageDimension>
void
SparseImage<TPixel, VImageDimension>
::FillBuffer(const PixelType& value)
{
  m_FillBufferValue = value;
  m_Container->GetPixelMap()->clear();
}


template<class TPixel, unsigned int VImageDimension>
void
SparseImage<TPixel, VImageDimension>
::SetPixelContainer(PixelContainer *container)
{
   if (m_Container != container)
     {
     m_Container = container;
     this->Modified();
     }
}


template<class TPixel, unsigned int VImageDimension>
void
SparseImage<TPixel, VImageDimension>
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

    // Copy from SliceContiguousImage< TPixel, dim >
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


template<class TPixel, unsigned int VImageDimension>
void
SparseImage<TPixel, VImageDimension>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "PixelContainer: " << std::endl;
  m_Container->Print(os, indent.GetNextIndent());
// m_Origin and m_Spacing are printed in the Superclass
}

} // end namespace itk

#endif
