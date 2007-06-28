/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkYAFFImageIO.cxx,v $
  Language:  C++
  Date:      $Date: 2007/03/29 18:39:28 $
  Version:   $Revision: 1.69 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include "itkExceptionObject.h"
#include "itkIOCommon.h"
#include "itkYAFFImageIO.h"

namespace itk
{

YAFFImageIO::YAFFImageIO()
{
} 

YAFFImageIO::~YAFFImageIO()
{
}

void YAFFImageIO::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}


bool YAFFImageIO::CanReadFile( const char* filename ) 
{ 
  // YAFF has no fear !!!
  return true;
}


void YAFFImageIO::ReadImageInformation()
{ 
  // YAFF only reads 8-bits unsigned char images.
  this->SetPixelType( SCALAR );
  this->SetComponentType( UCHAR );
} 


void YAFFImageIO::Read(void* buffer)
{ 
   // Add read here
} 


bool YAFFImageIO::CanWriteFile( const char * name )
{
  //
  // YAFF is not affraid of writing either !!
  // 
  return true;
}

  
void 
YAFFImageIO
::WriteImageInformation(void)
{
  // add writing here
}


/**
 *
 */
void 
YAFFImageIO
::Write( const void* buffer) 
{
}

/** Given a requested region, determine what could be the region that we can
 * read from the file. This is called the streamable region, which will be
 * smaller than the LargestPossibleRegion and greater or equal to the 
RequestedRegion */
ImageIORegion 
YAFFImageIO
::DetermineStreamableRegionFromRequestedRegion( const ImageIORegion & requested ) const
{
  std::cout << "YAFFImageIO::DetermineStreamableRegionFromRequestedRegion()" << std::endl;
  std::cout << "Requested region = " << requested << std::endl;
  //
  // The default implementations determines that the streamable region is
  // equal to the largest possible region of the image.
  //
  ImageIORegion streamableRegion(this->m_NumberOfDimensions);
  for( unsigned int i=0; i < this->m_NumberOfDimensions ; i++ )
    {
    streamableRegion.SetSize( i, this->m_Dimensions[i] );
    streamableRegion.SetIndex( i, 0 );
    }

  std::cout << "StreamableRegion = " << streamableRegion << std::endl;

  return streamableRegion;
}
 

} // end namespace itk
