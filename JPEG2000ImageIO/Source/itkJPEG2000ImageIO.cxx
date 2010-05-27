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

// for memset
#include <stdio.h>
#include <string.h>

#define USE_OPJ_DEPRECATED

extern "C" {
  #include "openjpeg.h"
  #include "j2k.h"
  #include "jp2.h"
  #include "convert.h"
}

//-----------------------------------------------------------------------------
 /**
 * \brief   routine for JPEG decompression
 * @param raw raw
 * @param inputdata inputdata
 * @param inputlength inputlength
 * @return 1 on success, 0 on error
 */

/** sample error callback expecting a FILE* client object */
extern "C"
{
  void openjpeg_error_callback(const char *msg, void *)
    {
    std::cerr << "Error in JPEG2000ImageIO" << msg << std::endl;
    }

/** sample warning callback expecting a FILE* client object */
  void openjpeg_warning_callback(const char *msg, void *)
   {
   std::cerr << "Warning in JPEG2000ImageIO" << msg << std::endl;
   }

/** sample debug callback expecting no client object */
  void openjpeg_info_callback(const char * msg, void *)
    {
    std::cerr << "Info in JPEG2000ImageIO" << msg << std::endl;
    }
}

//
// FIXME: Can we replace this with enums or const ints ?
//
#define J2K_CFMT 0
#define JP2_CFMT 1
#define JPT_CFMT 2
#define MJ2_CFMT 3
#define PXM_DFMT 0
#define PGX_DFMT 1
#define BMP_DFMT 2
#define YUV_DFMT 3


/*
 * Divide an integer by a power of 2 and round upwards.
 *
 * a divided by 2^b
 */
inline int int_ceildivpow2(int a, int b) {
  return (a + (1 << b) - 1) >> b;
}


namespace itk
{

JPEG2000ImageIO::JPEG2000ImageIO()
{
  this->SetNumberOfDimensions(2); // JPEG2000 is 2D. (by now...)
  this->SetNumberOfComponents(1); // Assume only one component. (FIXME)
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
  std::cout << "ReadImageInformation() " << std::endl;

  // JPEG2000 By NOW.... only reads 8-bits unsigned char images. (FIXME)
  this->SetPixelType( SCALAR );
  this->SetComponentType( UCHAR );

  FILE *fsrc = NULL;

  fsrc = fopen( this->m_FileName.c_str(), "rb");

  opj_dparameters_t parameters;  /* decompression parameters */

  if ( !fsrc ) 
    {
    itkExceptionMacro("ERROR -> failed to open for reading");
    }

  opj_stream_t * cio = NULL;

  cio = opj_stream_create_default_file_stream(fsrc,true);

  opj_codec_t * dinfo = NULL;  /* handle to a decompressor */

  opj_image_t *image = NULL;

  /* decode the code-stream */
  /* ---------------------- */

  std::string extension = itksys::SystemTools::GetFilenameLastExtension( this->m_FileName );

  if( extension == ".j2k" )
    {
    parameters.decod_format = J2K_CFMT;
    }

  if( extension == ".jp2" )
    {
    parameters.decod_format = JP2_CFMT;
    }


  switch (parameters.decod_format) 
    {
    case J2K_CFMT:
      {
      /* JPEG-2000 codestream */

      /* get a decoder handle */
      dinfo = opj_create_decompress(CODEC_J2K);
      break;
      }
    case JP2_CFMT:
      {
      /* JPEG 2000 compressed image data */
      /* get a decoder handle */
      dinfo = opj_create_decompress(CODEC_JP2);
      break;
      }
    case JPT_CFMT:
      {
      /* JPEG 2000, JPIP */
      /* get a decoder handle */
      dinfo = opj_create_decompress(CODEC_JPT);
      break;
      }
    default:
      fprintf(stderr, "skipping file..\n");
      opj_stream_destroy(cio);
      return;
    }
  /* catch events using our callbacks and give a local context */
    
  /* setup the decoder decoding parameters using user parameters */
  opj_setup_decoder(dinfo, &parameters);


  OPJ_INT32 l_tile_x0,l_tile_y0;

  OPJ_UINT32 l_tile_width,l_tile_height,l_nb_tiles_x,l_nb_tiles_y;

  /* decode the stream and fill the image structure */
  /*    if (*indexfilename)        // If need to extract codestream information
      image = opj_decode_with_info(dinfo, cio, &cstr_info);
    else
    */
  bool bResult = opj_read_header(
    dinfo,
    &image,
    &l_tile_x0,
    &l_tile_y0,
    &l_tile_width,
    &l_tile_height,
    &l_nb_tiles_x,
    &l_nb_tiles_y,
    cio);
  image = opj_decode(dinfo, cio);

  bResult = bResult && (image != 00);
  bResult = bResult && opj_end_decompress(dinfo,cio);

  if ( !image ) 
    {
    opj_destroy_codec(dinfo);
    opj_stream_destroy(cio);
    fclose(fsrc);
    itkExceptionMacro("ERROR -> j2k_to_image: failed to decode image!");
    }

std::cout << "image->x1 = " << image->x1 << std::endl;
std::cout << "image->y1 = " << image->y1 << std::endl;

  this->SetDimensions( 0, image->x1 );
  this->SetDimensions( 1, image->y1 );

  this->SetSpacing( 0, 1.0 );  // FIXME : Get the real pixel resolution.;
  this->SetSpacing( 1, 1.0 );  // FIXME : Get the real pixel resolution.


  /* close the byte stream */
  opj_stream_destroy(cio);
  fclose(fsrc);

  if (dinfo) 
    {
    opj_destroy_codec(dinfo);
    }
}


void JPEG2000ImageIO::Read( void * buffer)
{
  std::cout << "JPEG2000ImageIO::Read() Begin" << std::endl;

  FILE *fsrc = NULL;

  fsrc = fopen( this->m_FileName.c_str(), "rb");

  opj_dparameters_t parameters;  /* decompression parameters */

  if ( !fsrc ) 
    {
    itkExceptionMacro("ERROR -> failed to open for reading");
    }

  opj_stream_t * cio = NULL;

  cio = opj_stream_create_default_file_stream(fsrc,true);

  opj_codec_t * dinfo = NULL;  /* handle to a decompressor */

  opj_image_t *image = NULL;

  /* decode the code-stream */
  /* ---------------------- */

  std::string extension = itksys::SystemTools::GetFilenameLastExtension( this->m_FileName );

  if( extension == ".j2k" )
    {
    parameters.decod_format = J2K_CFMT;
    }

  if( extension == ".jp2" )
    {
    parameters.decod_format = JP2_CFMT;
    }


  switch (parameters.decod_format) 
    {
    case J2K_CFMT:
      {
      /* JPEG-2000 codestream */

      /* get a decoder handle */
      dinfo = opj_create_decompress(CODEC_J2K);
      break;
      }
    case JP2_CFMT:
      {
      /* JPEG 2000 compressed image data */
      /* get a decoder handle */
      dinfo = opj_create_decompress(CODEC_JP2);
      break;
      }
    case JPT_CFMT:
      {
      /* JPEG 2000, JPIP */
      /* get a decoder handle */
      dinfo = opj_create_decompress(CODEC_JPT);
      break;
      }
    default:
      fprintf(stderr, "skipping file..\n");
      opj_stream_destroy(cio);
      return;
    }
  /* catch events using our callbacks and give a local context */
    
  /* setup the decoder decoding parameters using user parameters */
  opj_setup_decoder(dinfo, &parameters);


  OPJ_INT32 l_tile_x0,l_tile_y0;

  OPJ_UINT32 l_tile_width,l_tile_height,l_nb_tiles_x,l_nb_tiles_y;

  /* decode the stream and fill the image structure */
  /*    if (*indexfilename)        // If need to extract codestream information
      image = opj_decode_with_info(dinfo, cio, &cstr_info);
    else
    */
  bool bResult = opj_read_header(
    dinfo,
    &image,
    &l_tile_x0,
    &l_tile_y0,
    &l_tile_width,
    &l_tile_height,
    &l_nb_tiles_x,
    &l_nb_tiles_y,
    cio);
  image = opj_decode(dinfo, cio);

  bResult = bResult && (image != 00);
  bResult = bResult && opj_end_decompress(dinfo,cio);

  if ( !image ) 
    {
    opj_destroy_codec(dinfo);
    opj_stream_destroy(cio);
    fclose(fsrc);
    itkExceptionMacro("ERROR -> j2k_to_image: failed to decode image!");
    }

  const size_t numberOfPixels = image->x1 * image->y1;

  unsigned char * charBuffer = (unsigned char *)buffer;
  size_t index = 0;

  // HERE, copy the buffer
  for ( size_t j = 0; j < numberOfPixels; j++) 
    {
    *charBuffer++ = image->comps[0].data[index];
    index++;
    }



  /* close the byte stream */
  opj_stream_destroy(cio);
  fclose(fsrc);

  if (dinfo) 
    {
    opj_destroy_codec(dinfo);
    }

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
