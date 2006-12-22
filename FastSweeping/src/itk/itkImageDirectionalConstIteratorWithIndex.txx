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
}
  
//----------------------------------------------------------------------
//  Is At End
//----------------------------------------------------------------------
template<class TImage>
bool
ImageDirectionalConstIteratorWithIndex<TImage>
::IsAtEnd()
{
  for(unsigned int i; i<ImageDimension; i++)
    {
    if( this->m_Directions[i] != -1 )
      {
      return false;
      }
    }
   return true;
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

}
 
  
//----------------------------------------------------------------------
//  Go to next direction
//----------------------------------------------------------------------
template<class TImage>
void
ImageDirectionalConstIteratorWithIndex<TImage>
::NextDirection()
{
  unsigned long binaryHelper = 0L;

  std::cout << "NextDirection() " << std::endl;
  std::cout << "ImageDimension " << ImageDimension << std::endl;

  for(unsigned int i=0; i<ImageDimension; i++)
    {
    unsigned long bit = ( this->m_Directions[i] == 1 ) ? 0 : 1;
    binaryHelper |= bit;
    binaryHelper << 1;
    }

  binaryHelper++;

  for(unsigned int j=0; j<ImageDimension; j++)
      {
      if( binaryHelper & 1 )
        {
        this->m_Directions[j] = -1;
        this->m_BeginIndex[j] = this->m_UnchangingEndIndex[j];
        this->m_EndIndex[j] = this->m_UnchangingBeginIndex[j];
        }
      else
        {
        this->m_Directions[j] = 1;
        this->m_BeginIndex[j] = this->m_UnchangingBeginIndex[j];
        this->m_EndIndex[j] = this->m_UnchangingEndIndex[j];
        }
      binaryHelper >> 1;
      }

  this->Superclass::GoToBegin();
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
    this->m_PositionIndex[ in  ] += this->m_Directions[ in ];
    if( this->m_PositionIndex[ in ] < this->m_EndIndex[ in ] )
      {
      // FIXME: A faster way to do this is to define a duplicate
      // offset table where we change +/- in NextDirection()
      this->m_Position += this->m_OffsetTable[in] * this->m_Directions[ in ];
      this->m_Remaining = true;
      break;
      }
    else 
      {
      this->m_Position -= this->m_OffsetTable[ in ] * this->m_Directions[ in ]  
        * ( static_cast<long>(this->m_Region.GetSize()[in])-1 );
      this->m_PositionIndex[ in ] = this->m_BeginIndex[ in ]; 
      }
    }

  if( !this->m_Remaining ) // It will not advance here otherwise
    {
    this->m_Position = this->m_End;
    }

  return *this;
}



} // end namespace itk

#endif
