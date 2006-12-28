/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageDirectionalConstIteratorWithIndex.txx,v $
  Language:  C++
  Date:      $Date: 2004/12/11 20:29:18 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkImageDirectionalConstIteratorWithIndex_txx
#define _itkImageDirectionalConstIteratorWithIndex_txx

#include "itkImageDirectionalConstIteratorWithIndex.h"

namespace itk
{
  
//----------------------------------------------------------------------
//  Constructor
//----------------------------------------------------------------------
template<class TImage>
ImageDirectionalConstIteratorWithIndex<TImage>
::ImageDirectionalConstIteratorWithIndex(const TImage *ptr,
                               const RegionType & region )
:ImageConstIteratorWithIndex<TImage>(ptr,region)
{
  this->m_Directions.Fill( 1 );

  this->m_UnchangingBeginIndex = this->m_BeginIndex;
  this->m_UnchangingEndIndex = this->m_EndIndex;
  this->m_BinaryHelper = 0L;
}
  
//----------------------------------------------------------------------
//  Is At End
//----------------------------------------------------------------------
template<class TImage>
bool
ImageDirectionalConstIteratorWithIndex<TImage>
::IsAtEnd()
{
  if( this->m_BinaryHelper == ( 1 << ImageDimension ) )
    {
    return true;
    }
  return false;
}

//----------------------------------------------------------------------
//  Is At End
//----------------------------------------------------------------------
template<class TImage>
bool
ImageDirectionalConstIteratorWithIndex<TImage>
::IsAtEndOfDirection()
{ 
  return this->Superclass::IsAtEnd();
}

//----------------------------------------------------------------------
//  Go to begin of iterations.
//----------------------------------------------------------------------
template<class TImage>
void
ImageDirectionalConstIteratorWithIndex<TImage>
::GoToBegin()
{
  this->Superclass::GoToBegin();
  this->m_Directions.Fill( 1 );
  this->m_BinaryHelper = 0L;
}
 
  
//----------------------------------------------------------------------
//  Go to next direction
//----------------------------------------------------------------------
template<class TImage>
void
ImageDirectionalConstIteratorWithIndex<TImage>
::NextDirection()
{
  this->m_BinaryHelper++;

  unsigned long binaryHelper = this->m_BinaryHelper;

  for(unsigned int j=0; j<ImageDimension; j++)
      {
      if( binaryHelper & 1 )
        {
        this->m_Remaining = true;
        this->m_Directions[j] = -1;
        this->m_BeginIndex[j] = this->m_UnchangingEndIndex[j]-1;
        this->m_EndIndex[j] = this->m_UnchangingBeginIndex[j]-1;
        }
      else
        {
        this->m_Directions[j] = 1;
        this->m_BeginIndex[j] = this->m_UnchangingBeginIndex[j];
        this->m_EndIndex[j] = this->m_UnchangingEndIndex[j];
        }
      binaryHelper >>= 1;
      }

  this->m_PositionIndex = this->m_BeginIndex;
  this->m_Position = this->m_Image->GetBufferPointer() + 
                     this->m_Image->ComputeOffset( this->m_PositionIndex );

}
 
//----------------------------------------------------------------------
//  Advance along the line
//----------------------------------------------------------------------
template<class TImage>
ImageDirectionalConstIteratorWithIndex<TImage> &
ImageDirectionalConstIteratorWithIndex<TImage>
::operator++()
{
  
  this->m_Remaining = false;
  for( unsigned int in=0; in<TImage::ImageDimension; in++ )
    {
    if( this->m_Directions[in] == 1 )
      {
      this->m_PositionIndex[ in ]++;
      if( this->m_PositionIndex[ in ] < this->m_EndIndex[ in ] )
        {
        this->m_Position += this->m_OffsetTable[in];
        this->m_Remaining = true;
        break;
        }
      else 
        {
        this->m_Position -= this->m_OffsetTable[ in ] *
            ( static_cast<long>(this->m_Region.GetSize()[in])-1 );
        this->m_PositionIndex[ in ] = this->m_BeginIndex[ in ]; 
        }
      }
    else
      {
      this->m_PositionIndex[ in ]--;
      if( this->m_PositionIndex[ in ] > this->m_EndIndex[ in ] )
        {
        this->m_Position -= this->m_OffsetTable[in];
        this->m_Remaining = true;
        break;
        }
      else 
        {
        this->m_Position += this->m_OffsetTable[ in ] * 
            ( static_cast<long>(this->m_Region.GetSize()[in])-1 );
        this->m_PositionIndex[ in ] = this->m_BeginIndex[ in ]; 
        }
      }
    }

  if( !this->m_Remaining ) // It will not advance here otherwise
    {
    this->m_Position = this->m_End;
    }

  return *this;
}

  
//----------------------------------------------------------------------
//  Define the size of the neigborhood
//----------------------------------------------------------------------
template<class TImage>
void
ImageDirectionalConstIteratorWithIndex<TImage>
::SetRadius(const unsigned long radius)
{
  m_Neigborhood.SetRadius( radius );
}

  
//----------------------------------------------------------------------
//  Return the first neighbor
//----------------------------------------------------------------------
template<class TImage>
typename ImageDirectionalConstIteratorWithIndex<TImage>::Iterator
ImageDirectionalConstIteratorWithIndex<TImage>
::GetFirstNeighborIterator()
{
   return m_Neigborhood.Begin();
}


  
//----------------------------------------------------------------------
//  Return the first neighbor (const version)
//----------------------------------------------------------------------
template<class TImage>
typename ImageDirectionalConstIteratorWithIndex<TImage>::ConstIterator
ImageDirectionalConstIteratorWithIndex<TImage>
::GetFirstNeighborIterator() const
{
   return m_Neigborhood.Begin();
}

 
//----------------------------------------------------------------------
//  Return the one-past-the-end neighbor
//----------------------------------------------------------------------
template<class TImage>
typename ImageDirectionalConstIteratorWithIndex<TImage>::Iterator
ImageDirectionalConstIteratorWithIndex<TImage>
::GetTerminalNeighborIterator()
{
   return m_Neigborhood.End();
}


  
//----------------------------------------------------------------------
//  Return the one-past-the-end neighbor (const version)
//----------------------------------------------------------------------
template<class TImage>
typename ImageDirectionalConstIteratorWithIndex<TImage>::ConstIterator
ImageDirectionalConstIteratorWithIndex<TImage>
::GetTerminalNeighborIterator() const
{
   return m_Neigborhood.End();
}



} // end namespace itk

#endif
