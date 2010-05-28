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
  //   opj_dparameters_t m_DecompressionParameters;
  opj_set_default_decoder_parameters(&m_DecompressionParameters);

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

  /* decompression parameters */

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
    m_DecompressionParameters.decod_format = J2K_CFMT;
    }

  if( extension == ".jp2" )
    {
    m_DecompressionParameters.decod_format = JP2_CFMT;
    }

  if( extension == ".jpt" )
    {
    m_DecompressionParameters.decod_format = JPT_CFMT;
    }

  switch (m_DecompressionParameters.decod_format)
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
  /* No reading of image information done */
  opj_setup_decoder(dinfo, &m_DecompressionParameters);

  // Image parameters - first tile
  OPJ_INT32 l_tile_x0,l_tile_y0;
  // Image parameters - tile width, height and number of tiles
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

//  image = opj_decode(dinfo, cio); // FIXME : should this be here ?

  std::cout << "l_tile_x0 = " << l_tile_x0 << std::endl;
  std::cout << "l_tile_y0 = " << l_tile_y0 << std::endl;
  std::cout << "l_tile_height = " << l_tile_height << std::endl;
  std::cout << "l_tile_width = " << l_tile_width << std::endl;
  std::cout << "l_nb_tiles_x = " << l_nb_tiles_x << std::endl;
  std::cout << "l_nb_tiles_y = " << l_nb_tiles_y << std::endl;

//  bResult = bResult && (image != 00);
//  bResult = bResult && opj_end_decompress(dinfo,cio);  // FIXME : should this be here ?

  if ( !image )
    {
    opj_destroy_codec(dinfo);
    opj_stream_destroy(cio);
    fclose(fsrc);
    itkExceptionMacro("ERROR -> j2k_to_image: failed to decode image!");
    }

  // FIXME: bResult is not used.  We should check for it and maybe throw an exception.

std::cout << "image->x1 = " << image->x1 << std::endl;
std::cout << "image->y1 = " << image->y1 << std::endl;

  this->SetDimensions( 0, image->x1 );
  this->SetDimensions( 1, image->y1 );

  this->SetSpacing( 0, 1.0 );  // FIXME : Get the real pixel resolution.
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
  switch (m_DecompressionParameters.decod_format)
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
  opj_setup_decoder(dinfo, &this->m_DecompressionParameters);


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

//   bResult = bResult && (image != 00);
//   bResult = bResult && opj_end_decompress(dinfo,cio);

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
  std::cout << " START COPY BUFFER" << std::endl;
  for ( size_t j = 0; j < numberOfPixels; j++)
    {
    *charBuffer++ = image->comps[0].data[index];
    index++;
    }
  std::cout << " END COPY BUFFER" << std::endl;

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

  if( extension == ".jpt" )
    {
    return true;
    }

  return false;
}


void
JPEG2000ImageIO
::WriteImageInformation(void)
{
  std::cout << "WriteImageInformation()" << std::endl;

  // the IORegion is not requred to be set so we must use GetNumberOfDimensions
  if ( this->GetNumberOfDimensions() != 2 )
    {
    itkExceptionMacro(<<"JPEG 2000 Writer can only write 2-dimensional images");
    }

  if ( this->GetComponentType() != UCHAR
    && this->GetComponentType() != UINT)
    {
    itkExceptionMacro(<<"JPEG 2000 supports unsigned char/int only");
    }
}


/**
 *
 */
void
JPEG2000ImageIO
::Write( const void* buffer)
{
  std::cout << "Write()" << std::endl;

  bool bSuccess;

  opj_cparameters_t parameters;
  opj_set_default_encoder_parameters(&parameters);

//--------------------------------------------------------
  // Copy the contents into the image structure
  int i, w, h , numcomps = 1;
  w = this->m_Dimensions[0];
  h = this->m_Dimensions[1];

  OPJ_COLOR_SPACE color_space = CLRSPC_GRAY;
  opj_image_cmptparm_t cmptparm[3];

  /* initialize image components */
  memset(&cmptparm[0], 0, sizeof(opj_image_cmptparm_t));
  cmptparm[0].prec = 8;
  cmptparm[0].bpp = 8;
  cmptparm[0].sgnd = 0;
  cmptparm[0].dx = 1;
  cmptparm[0].dy = 1;
  cmptparm[0].w =  w;
  cmptparm[0].h = h;

  opj_image_t *image = NULL;
  image = opj_image_create( numcomps, &cmptparm[0], color_space);
  if(!image)
  {
    itkExceptionMacro(<<"Image buffer not created");
  }

  image->numcomps = this->GetNumberOfComponents();

  int subsampling_dx = parameters.subsampling_dx;
  int subsampling_dy = parameters.subsampling_dy;
  image->x0 = parameters.image_offset_x0;
  image->y0 = parameters.image_offset_y0;
  image->x1 = !image->x0 ? (w - 1) * subsampling_dx + 1 : image->x0 + (w - 1) * subsampling_dx + 1;
  image->y1 = !image->y0 ? (h - 1) * subsampling_dy + 1 : image->y0 + (h - 1) * subsampling_dy + 1;

  // HERE, copy the buffer
  unsigned char * charBuffer = (unsigned char *)buffer;
  size_t index = 0;
  int numberOfPixels = w*h;
  std::cout << " START COPY BUFFER" << std::endl;
  for ( size_t j = 0; j < numberOfPixels; j++)
    {
    image->comps[0].data[index] = charBuffer[index];
    index++;
    }
  std::cout << " END COPY BUFFER" << std::endl;
//--------------------------------------------------------------------

  /* Create comment for codestream */
  if(parameters.cp_comment == NULL) {
    const char comment[] = "Created by OpenJPEG version ";
    const size_t clen = strlen(comment);
    const char *version = opj_version();
/* UniPG>> */
#ifdef USE_JPWL
    parameters.cp_comment = (char*)malloc(clen+strlen(version)+11);
    sprintf(parameters.cp_comment,"%s%s with JPWL", comment, version);
#else
    parameters.cp_comment = (char*)malloc(clen+strlen(version)+1);
    sprintf(parameters.cp_comment,"%s%s", comment, version);
#endif
/* <<UniPG */
  }

  strncpy(parameters.outfile, this->m_FileName.c_str(), sizeof(parameters.outfile)-1);
  std::string extension = itksys::SystemTools::GetFilenameLastExtension( this->m_FileName.c_str() );

  opj_codec_t* cinfo = NULL;
  if( extension == ".j2k" )
    {
    cinfo = opj_create_compress(CODEC_J2K);
    parameters.cod_format = J2K_CFMT;
    }

  if( extension == ".jp2" )
    {
    cinfo = opj_create_compress(CODEC_JP2);
    parameters.cod_format = JP2_CFMT;
    }

  if ( this->GetNumberOfComponents() == 3 )
    parameters.tcp_mct = 1;
  else
    parameters.tcp_mct = 0;

  opj_setup_encoder(cinfo, &parameters, image);

  FILE *f = NULL;
  f = fopen(parameters.outfile, "wb");
  if (! f)
  {
    fprintf(stderr, "failed to encode image\n");
    return;
  }

  /* open a byte stream for writing */
  /* allocate memory for all tiles */
  opj_stream_t *cio = 00;
  cio = opj_stream_create_default_file_stream(f,false);
  if (! cio)
  {
    fprintf(stderr, "no file stream opened\n");
    return;
  }

  /* encode the image */
  /*if (*indexfilename)         // If need to extract codestream information
    bSuccess = opj_encode_with_info(cinfo, cio, image, &cstr_info);
  else*/
  std::cout << "Before compressing " << cinfo << ' ' << image << ' ' << cio << ' ' << f << std::endl;
//   bSuccess = opj_start_compress(cinfo,image,cio);
//   bSuccess = bSuccess && opj_encode(cinfo, cio);
//   bSuccess = bSuccess && opj_end_compress(cinfo, cio);
//
//   if (!bSuccess)
//   {
//     opj_stream_destroy(cio);
//     fclose(f);
//     fprintf(stderr, "failed to encode image\n");
//     return;
//   }
//
//   fprintf(stderr,"Generated outfile %s\n",parameters.outfile);
//   /* close and free the byte stream */
//   opj_stream_destroy(cio);
//   fclose(f);
//
// //   /* Write the index to disk */
// //   if (*indexfilename)
// //   {
// //     bSuccess = write_index_file(&cstr_info, indexfilename);
// //     if (bSuccess)
// //     {
// //       fprintf(stderr, "Failed to output index file\n");
// //     }
// //   }
//
//   /* free remaining compression structures */
//   opj_destroy_codec(cinfo);
// //   if (*indexfilename)
// //     opj_destroy_cstr_info(&cstr_info);
//
//   /* free image data */
//   opj_image_destroy(image);
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
