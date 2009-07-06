/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSingleBitBinaryImage.txx,v $
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
#ifndef __itkSingleBitBinaryImage_txx
#define __itkSingleBitBinaryImage_txx

#include "itkSingleBitBinaryImage.h"
#include "itkProcessObject.h"

namespace itk
{

template<unsigned int VImageDimension>
SingleBitBinaryImage<VImageDimension>
::SingleBitBinaryImage()
{
  m_Buffer = PixelContainer::New();
}


template<unsigned int VImageDimension>
void
SingleBitBinaryImage<VImageDimension>
::Allocate()
{
  unsigned long num;

  this->ComputeOffsetTable();
  num = this->GetOffsetTable()[VImageDimension];
  num = ( num % 32 == 0) ? num / 32 : (num / 32) + 1;

  m_Buffer->Reserve(num);
}


template<unsigned int VImageDimension>
void
SingleBitBinaryImage<VImageDimension>
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
  m_Buffer = PixelContainer::New();
}


template<unsigned int VImageDimension>
void 
SingleBitBinaryImage<VImageDimension>
::FillBuffer (const PixelType& value)
{
  InternalPixelType fillValue = value ?
    NumericTraits<InternalPixelType>::max() :
    NumericTraits<InternalPixelType>::Zero;

  for(unsigned int i=0; i<m_Buffer->Size(); i++)
    {
    (*m_Buffer)[i] = fillValue;
    }
}


template<unsigned int VImageDimension>
void
SingleBitBinaryImage<VImageDimension>
::SetPixelContainer(PixelContainer *container)
{
  if (m_Buffer != container)
    {
    m_Buffer = container;
    this->Modified();
    }
}


template<unsigned int VImageDimension>
void
SingleBitBinaryImage<VImageDimension>
::Graft(const DataObject *data)
{
  // call the superclass' implementation
  Superclass::Graft( data );

  if ( data )
    {
    // Attempt to cast data to an SingleBitBinaryImage
    const Self * imgData;

    try
      {
      imgData = dynamic_cast<const Self *>( data );
      }
    catch( ... )
      {
      return;
      }

    if ( imgData )
      {
      // Now copy anything remaining that is needed
      this->SetPixelContainer( const_cast< PixelContainer * >
                                  (imgData->GetPixelContainer()) );
      }
    else
      {
      // pointer could not be cast back down
      itkExceptionMacro( << "itk::SingleBitBinaryImage::Graft() cannot cast "
                         << typeid(data).name() << " to "
                         << typeid(const Self *).name() );
      }
    }
}


template<unsigned int VImageDimension>
void
SingleBitBinaryImage<VImageDimension>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "PixelContainer: " << std::endl;
  m_Buffer->Print(os, indent.GetNextIndent());

  // m_Origin and m_Spacing are printed in the Superclass
}

} // end namespace itk

#endif
