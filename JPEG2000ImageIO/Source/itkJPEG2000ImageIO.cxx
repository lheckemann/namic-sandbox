/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkJPEG2000ImageIO.cxx,v $
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
#include "itkJPEG2000ImageIO.h"
#include "itksys/SystemTools.hxx"

namespace itk
{

JPEG2000ImageIO::JPEG2000ImageIO()
{
  this->SetNumberOfDimensions(2); // JPEG2000 is 2D.
  this->SetNumberOfComponents(1); // JPEG2000 only has one component. (FIXME)
} 

JPEG2000ImageIO::~JPEG2000ImageIO()
{
}

void JPEG2000ImageIO::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

bool JPEG2000ImageIO::CanReadFile( const char* filename ) 
{ 
  std::cout << "JPEG2000ImageIO::CanReadFile() " << std::endl;

  //
  // If the file exists, and have extension .j2k or jp2, then we are good to read it.
  //
  if( !itksys::SystemTools::FileExists( filename ) )
    {
    std::cout << "File doesn't exist" << std::endl;
    return false;
    }

  std::string extension = itksys::SystemTools::GetFilenameLastExtension( filename );

  if( extension == ".j2k" )
    {
    return true;
    }

   if( extension == ".jp2" )
    {
    return true;
    }
 
  return false;
}


void JPEG2000ImageIO::ReadImageInformation()
{ 
  // JPEG2000 only reads 8-bits unsigned char images.
  this->SetPixelType( SCALAR );
  this->SetComponentType( UCHAR );

  this->m_InputStream.open( this->m_FileName.c_str() );
 
  if( this->m_InputStream.fail() )
    {
    itkExceptionMacro("Failed to open file " << this->m_InputStream );
    }

  unsigned int nx;
  unsigned int ny;

  double dx;
  double dy;

  this->m_InputStream >> nx >> ny;
  this->m_InputStream >> dx >> dy;

  std::string rawFileName;
  this->m_InputStream >> rawFileName;
 
  this->m_InputStream.close();

  std::cout << "::Read() filename= " << this->m_FileName << std::endl;
  this->m_RawDataFilename = itksys::SystemTools::GetFilenamePath( this->m_FileName );
  this->m_RawDataFilename += '/';
  this->m_RawDataFilename += rawFileName;

  if( !itksys::SystemTools::FileExists( this->m_RawDataFilename.c_str() ) )
    {
    itkExceptionMacro("Raw data file does not exist " << this->m_RawDataFilename );
    }
 
  this->SetDimensions( 0, nx );
  this->SetDimensions( 1, ny );

  this->SetSpacing( 0, dx );
  this->SetSpacing( 1, dy );
}


void JPEG2000ImageIO::Read( void * buffer)
{ 
  std::cout << "JPEG2000ImageIO::Read() Begin" << std::endl;

  this->m_InputStream.open( this->m_RawDataFilename.c_str() );

  const unsigned int nx = this->GetDimensions( 0 );
  const unsigned int ny = this->GetDimensions( 1 );
 
  ImageIORegion regionToRead = this->GetIORegion();

  ImageIORegion::SizeType  size  = regionToRead.GetSize();
  ImageIORegion::IndexType start = regionToRead.GetIndex();

  const unsigned int mx = size[0];
  const unsigned int my = size[1];

  std::cout << "largest    region size = " << nx << " " << ny << std::endl;
  std::cout << "streamable region size = " << mx << " " << my << std::endl;

  //  const unsigned int sx = start[0];
  //  const unsigned int sy = start[1];

  //  char * inptr = static_cast< char * >( buffer );

  // FIXME : Read the real data
 
  std::cout << "JPEG2000ImageIO::Read() End" << std::endl;
} 


bool JPEG2000ImageIO::CanWriteFile( const char * filename )
{
  std::string extension = itksys::SystemTools::GetFilenameLastExtension( filename );

  if( extension == ".j2k" )
    {
    return true;
    }

   if( extension == ".jp2" )
    {
    return true;
    }
 
  return false;
}

  
void 
JPEG2000ImageIO
::WriteImageInformation(void)
{
  // add writing here
}


/**
 *
 */
void 
JPEG2000ImageIO
::Write( const void* buffer) 
{
}

/** Given a requested region, determine what could be the region that we can
 * read from the file. This is called the streamable region, which will be
 * smaller than the LargestPossibleRegion and greater or equal to the 
RequestedRegion */
ImageIORegion 
JPEG2000ImageIO
::GenerateStreamableReadRegionFromRequestedRegion( const ImageIORegion & requested ) const
{
  std::cout << "JPEG2000ImageIO::GenerateStreamableReadRegionFromRequestedRegion()" << std::endl;
  std::cout << "Requested region = " << requested << std::endl;
  //
  // JPEG2000 is the ultimate streamer.
  //
  ImageIORegion streamableRegion = requested;

  std::cout << "StreamableRegion = " << streamableRegion << std::endl;

  return streamableRegion;
}
 

} // end namespace itk
