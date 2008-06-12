/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImportSliceContiguousImageContainer.txx,v $
  Language:  C++
  Date:      $Date: 2007-12-12 15:49:13 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkImportSliceContiguousImageContainer_txx
#define _itkImportSliceContiguousImageContainer_txx

#include "itkImportSliceContiguousImageContainer.h"

namespace itk
{

template <typename TElementIdentifier, typename TElement>
ImportSliceContiguousImageContainer<TElementIdentifier , TElement>
::ImportSliceContiguousImageContainer()
{
  m_ImportPointer = 0;
  m_ContainerManageMemory = true;
  m_Capacity = 0;
  m_Size = 0;
}

  
template <typename TElementIdentifier, typename TElement>
ImportSliceContiguousImageContainer< TElementIdentifier , TElement >
::~ImportSliceContiguousImageContainer()
{
  if (m_ImportPointer && m_ContainerManageMemory)
    {
    delete [] m_ImportPointer;
    }

  if( m_ContainerManageMemory )
    {
    typename SliceArrayType::iterator itr = m_SlicesArray.begin();
    typename SliceArrayType::iterator end = m_SlicesArray.end();
    while( itr != end )
      {
      TElement * sliceToBeReleased = *itr;
      delete [] sliceToBeReleased;
      ++itr;
      }
    m_SlicesArray.clear();
    }
}


/**
 * Tell the container to allocate enough memory to allow at least
 * as many elements as the size given to be stored.  
 */
template <typename TElementIdentifier, typename TElement>
void
ImportSliceContiguousImageContainer< TElementIdentifier , TElement >
::Reserve(ElementIdentifier size)
{
  // TODO:
  //if (m_ImportPointer)
  //  {
  //  if (size > m_Capacity)
  //    {
  //    TElement* temp = this->AllocateElements(size);
  //    // only copy the portion of the data used in the old buffer
  //    memcpy(temp, m_ImportPointer, m_Size*sizeof(TElement));
  //    if (m_ImportPointer && m_ContainerManageMemory)
  //      {
  //      delete [] m_ImportPointer;
  //      }
  //    m_ImportPointer = temp;
  //    m_ContainerManageMemory = true;
  //    m_Capacity = size;
  //    m_Size = size;
  //    this->Modified();
  //    }
  //  else
  //    {
  //    m_Size = size;
  //    this->Modified();
  //    }
  //  }
  //else
  //  {
  //  m_ImportPointer = this->AllocateElements(size);
  //  m_Capacity = size;
  //  m_Size = size;
  //  m_ContainerManageMemory = true;
  //  this->Modified();
  //  }
}


/**
 * Tell the container to try to minimize its memory usage for storage of
 * the current number of elements.  
 */
template <typename TElementIdentifier, typename TElement>
void
ImportSliceContiguousImageContainer< TElementIdentifier , TElement >
::Squeeze(void)
{
  // TODO:  
  //if (m_ImportPointer)
  //  {
  //  if (m_Size < m_Capacity)
  //    {
  //    TElement* temp = this->AllocateElements(m_Size);
  //    memcpy(temp, m_ImportPointer, m_Size*sizeof(TElement));
  //    if (m_ContainerManageMemory)
  //      {
  //      delete [] m_ImportPointer;
  //      }
  //    m_ImportPointer = temp;
  //    m_ContainerManageMemory = true;
  //    m_Capacity = m_Size;

  //    this->Modified();
  //    }
  //  }
}


/**
 * Tell the container to try to minimize its memory usage for storage of
 * the current number of elements.  
 */
template <typename TElementIdentifier, typename TElement>
void
ImportSliceContiguousImageContainer< TElementIdentifier , TElement >
::Initialize(void)
{
  // TODO:
  //if (m_ImportPointer)
  //  {
  //  if (m_ContainerManageMemory)
  //    {
  //    delete [] m_ImportPointer;
  //    }
  //  m_ImportPointer = 0;
  //  m_ContainerManageMemory = true;
  //  m_Capacity = 0;
  //  m_Size = 0;
  //  
  //  this->Modified();
  //  }
}


template <typename TElementIdentifier, typename TElement>
void
ImportSliceContiguousImageContainer< TElementIdentifier , TElement >
::SetImportPointersForSlices( SliceArrayType& slices,
                              TElementIdentifier numPerSlice,
                              TElementIdentifier numSlices,
                              bool LetContainerManageMemory )
{ 
  // Free slices
  if( m_ContainerManageMemory )
    {
    typename SliceArrayType::iterator itr = m_SlicesArray.begin();
    typename SliceArrayType::iterator end = m_SlicesArray.end();
    while( itr != end )
      {
      TElement * sliceToBeReleased = *itr;
      delete [] sliceToBeReleased;
      ++itr;
      }
    m_SlicesArray.clear();
    }

  // Set ivars
  m_ContainerManageMemory = LetContainerManageMemory;
  m_Capacity = numPerSlice*numSlices;
  m_Size = numPerSlice*numSlices;

  // Copy slices
  m_SlicesArray.resize( slices.size() );
  std::copy( slices.begin(), slices.end(), m_SlicesArray.begin() );

  this->Modified();
}


template <typename TElementIdentifier, typename TElement>
TElement* ImportSliceContiguousImageContainer< TElementIdentifier , TElement >
::AllocateElements(ElementIdentifier size) const
{
  // TODO:
  //// Encapsulate all image memory allocation here to throw an
  //// exception when memory allocation fails even when the compiler
  //// does not do this by default.
  //TElement* data;
  //try
  //  {
  //  data = new TElement[size];
  //  }
  //catch(...)
  //  {
  //  data = 0;
  //  }
  //if(!data)
  //  {
  //  // We cannot construct an error string here because we may be out
  //  // of memory.  Do not use the exception macro.
  //  throw MemoryAllocationError(__FILE__, __LINE__,
  //                              "Failed to allocate memory for image.",
  //                              ITK_LOCATION);
  //  }
  //return data;
  return NULL;
}

template <typename TElementIdentifier, typename TElement>
void
ImportSliceContiguousImageContainer< TElementIdentifier , TElement >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Container manages memory: "
     << (m_ContainerManageMemory ? "true" : "false") << std::endl;
  os << indent << "Size: " << m_Size << std::endl;
  os << indent << "Capacity: " << m_Capacity << std::endl;
}

} // end namespace itk

#endif
