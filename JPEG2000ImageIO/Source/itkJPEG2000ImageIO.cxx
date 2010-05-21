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
  this->SetNumberOfDimensions(3); // JPEG2000 is 3D.
  this->SetNumberOfComponents(1); // JPEG2000 only has one component.
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
  // If the file exists, and have extension .yaff, then we are good to read it.
  //
  if( !itksys::SystemTools::FileExists( filename ) )
    {
    std::cout << "File doesn't exist" << std::endl;
    return false;
    }
std::cout << itksys::SystemTools::GetFilenameLastExtension( filename ) << std::endl;
  if( itksys::SystemTools::GetFilenameLastExtension( filename ) != ".yaff" )
    {
    std::cout << "Wrong extension" << std::endl;
    return false;
    }
 
  return true;
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
  unsigned int nz;

  double dx;
  double dy;
  double dz;

  this->m_InputStream >> nx >> ny >> nz;
  this->m_InputStream >> dx >> dy >> dz;

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
  this->SetDimensions( 2, nz );

  this->SetSpacing( 0, dx );
  this->SetSpacing( 1, dy );
  this->SetSpacing( 2, dz );
}


void JPEG2000ImageIO::Read( void * buffer)
{ 
  std::cout << "JPEG2000ImageIO::Read() Begin" << std::endl;

  this->m_InputStream.open( this->m_RawDataFilename.c_str() );

  const unsigned int nx = this->GetDimensions( 0 );
  const unsigned int ny = this->GetDimensions( 1 );
  const unsigned int nz = this->GetDimensions( 2 );
 
  ImageIORegion regionToRead = this->GetIORegion();

  ImageIORegion::SizeType  size  = regionToRead.GetSize();
  ImageIORegion::IndexType start = regionToRead.GetIndex();

  const unsigned int mx = size[0];
  const unsigned int my = size[1];
  const unsigned int mz = size[2];

  const unsigned int sx = start[0];
  const unsigned int sy = start[1];
  const unsigned int sz = start[2];

  std::cout << "largest    region size = " << nx << " " << ny << " " << nz << std::endl;
  std::cout << "streamable region size = " << mx << " " << my << " " << mz << std::endl;

  char * inptr = static_cast< char * >( buffer );

  unsigned int pos = sz * ( nx * ny ) + sy * nx + sx;

  this->m_InputStream.seekg( pos, std::ios_base::beg );

  for( unsigned int iz = 0; iz < mz; iz++ )
    {
    for( unsigned int iy = 0; iy < my; iy++ )
      {
      this->m_InputStream.read( inptr, mx );
      inptr += mx;
      this->m_InputStream.seekg( nx - mx, std::ios_base::cur );
      }
    this->m_InputStream.seekg( ( ny - my ) * nx, std::ios_base::cur );
    }
  
  this->m_InputStream.close();
 
  std::cout << "JPEG2000ImageIO::Read() End" << std::endl;
} 


bool JPEG2000ImageIO::CanWriteFile( const char * name )
{
  //
  // JPEG2000 is not affraid of writing either !!
  // 
  return true;
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
