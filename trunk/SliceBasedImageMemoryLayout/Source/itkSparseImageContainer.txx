/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSparseImageContainer.txx,v $
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
#ifndef _itkSparseImageContainer_txx
#define _itkSparseImageContainer_txx

#include "itkSparseImageContainer.h"

namespace itk
{

template <typename TElementIdentifier, typename TElement>
SparseImageContainer<TElementIdentifier , TElement>
::SparseImageContainer()
{
  m_ContainerManageMemory = true;
}


template <typename TElementIdentifier, typename TElement>
SparseImageContainer< TElementIdentifier , TElement >
::~SparseImageContainer()
{
  if( m_ContainerManageMemory )
    {
    m_PixelMap.clear();
    }
}


/**
 * Tell the container to allocate enough memory to allow at least
 * as many elements as the size given to be stored.
 */
template <typename TElementIdentifier, typename TElement>
void
SparseImageContainer< TElementIdentifier , TElement >
::Reserve(void)
{
  // Nothing to do, a map can not reserve elements
}


/**
 * Tell the container to try to minimize its memory usage for storage of
 * the current number of elements.
 */
template <typename TElementIdentifier, typename TElement>
void
SparseImageContainer< TElementIdentifier , TElement >
::Squeeze(void)
{
  // Nothing to do, a map can not be squeezed
}


/**
 * Tell the container to release any of its allocated memory.
 */
template <typename TElementIdentifier, typename TElement>
void
SparseImageContainer< TElementIdentifier , TElement >
::Initialize(void)
{
  if ( m_PixelMap.size() > 0 )
    {
    if( m_ContainerManageMemory )
      {
      m_PixelMap.clear();
      }
    m_ContainerManageMemory = true;
    this->Modified();
    }
}


template <typename TElementIdentifier, typename TElement>
void
SparseImageContainer< TElementIdentifier , TElement >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Container manages memory: "
     << (m_ContainerManageMemory ? "true" : "false") << std::endl;
}

} // end namespace itk

#endif
