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


namespace itk
{

JPEG2000ImageIO::JPEG2000ImageIO()
{
  //   opj_dparameters_t m_DecompressionParameters;
  opj_set_default_decoder_parameters(&m_DecompressionParameters);

  this->SetNumberOfDimensions(2); // JPEG2000 is 2D. (by now...)
  this->SetNumberOfComponents(1);

  this->m_Dinfo = NULL;
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
  // If the file exists, and have extension .j2k or jp2 or jpt, then we are good to read it.
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

  if( extension == ".jpt" )
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

  FILE * l_file = NULL;

   l_file = fopen( this->m_FileName.c_str(), "rb");

  /* decompression parameters */

  if ( ! l_file )
    {
    itkExceptionMacro("ERROR -> failed to open for reading");
    }

  opj_stream_t * cio = NULL;

  cio = opj_stream_create_default_file_stream( l_file,true);

  this->m_Dinfo = NULL;  /* handle to a decompressor */

  opj_image_t * l_image = NULL;

  /* decode the code-stream */
  /* ---------------------- */

  std::string extension = itksys::SystemTools::GetFilenameLastExtension( this->m_FileName );

  if( extension == ".j2k" )
    {
    m_DecompressionParameters.decod_format = Self::J2K_CFMT;
    }

  if( extension == ".jp2" )
    {
    m_DecompressionParameters.decod_format = Self::JP2_CFMT;
    }

  if( extension == ".jpt" )
    {
    m_DecompressionParameters.decod_format = Self::JPT_CFMT;
    }

  switch (m_DecompressionParameters.decod_format)
    {
    case Self::J2K_CFMT:
      {
      /* JPEG-2000 codestream */

      /* get a decoder handle */
      this->m_Dinfo = opj_create_decompress(CODEC_J2K);
      break;
      }
    case Self::JP2_CFMT:
      {
      /* JPEG 2000 compressed image data */
      /* get a decoder handle */
      this->m_Dinfo = opj_create_decompress(CODEC_JP2);
      break;
      }
    case Self::JPT_CFMT:
      {
      /* JPEG 2000, JPIP */
      /* get a decoder handle */
      this->m_Dinfo = opj_create_decompress(CODEC_JPT);
      break;
      }
    default:
      itkExceptionMacro("skipping file..\n");
      opj_stream_destroy(cio);
      return;
    }
  /* catch events using our callbacks and give a local context */

  /* setup the decoder decoding parameters using user parameters */
  /* No reading of image information done */
  opj_setup_decoder( this->m_Dinfo, &m_DecompressionParameters);

  // Image parameters - first tile
  OPJ_INT32 l_tile_x0;
  OPJ_INT32 l_tile_y0;

  // Image parameters - tile width, height and number of tiles
  OPJ_UINT32 l_tile_width;
  OPJ_UINT32 l_tile_height;
  OPJ_UINT32 l_nb_tiles_x;
  OPJ_UINT32 l_nb_tiles_y;

  bool bResult = opj_read_header(
    this->m_Dinfo,
    & l_image,
    &l_tile_x0,
    &l_tile_y0,
    &l_tile_width,
    &l_tile_height,
    &l_nb_tiles_x,
    &l_nb_tiles_y,
    cio);


  if ( !bResult )
    {
    itkExceptionMacro("Error while reading image header. opj_read_header returns false");
    }


  if ( ! l_image )
    {
    itkExceptionMacro("Error while reading image header");
    }

  std::cout << "Number of Components = " << l_image->numcomps << std::endl;
  this->SetNumberOfComponents(  l_image->numcomps );

  std::cout << "l_tile_x0 = " << l_tile_x0 << std::endl;
  std::cout << "l_tile_y0 = " << l_tile_y0 << std::endl;
  std::cout << "l_tile_height = " << l_tile_height << std::endl;
  std::cout << "l_tile_width = " << l_tile_width << std::endl;
  std::cout << "l_nb_tiles_x = " << l_nb_tiles_x << std::endl;
  std::cout << "l_nb_tiles_y = " << l_nb_tiles_y << std::endl;

  if ( ! l_image )
    {
    opj_destroy_codec( this->m_Dinfo );
    this->m_Dinfo = NULL;
    opj_stream_destroy(cio);
    fclose( l_file );
    itkExceptionMacro("ERROR -> j2k_to_image: failed to decode image!");
    }

  std::cout << "image->x1 = " <<  l_image->x1 << std::endl;
  std::cout << "image->y1 = " <<  l_image->y1 << std::endl;

  this->SetDimensions( 0,  l_image->x1 );
  this->SetDimensions( 1,  l_image->y1 );

  this->SetSpacing( 0, 1.0 );  // FIXME : Get the real pixel resolution.
  this->SetSpacing( 1, 1.0 );  // FIXME : Get the real pixel resolution.

  /* close the byte stream */
  opj_stream_destroy(cio);
  fclose( l_file );

  if ( this->m_Dinfo )
    {
    opj_destroy_codec( this->m_Dinfo );
    this->m_Dinfo = NULL;
    }
}


void JPEG2000ImageIO::Read( void * buffer)
{
  std::cout << "JPEG2000ImageIO::Read() Begin" << std::endl;

  FILE * l_file = NULL;

  l_file = fopen( this->m_FileName.c_str(), "rb");

  if ( ! l_file )
    {
    itkExceptionMacro("ERROR -> failed to open for reading");
    }

  opj_stream_t * l_stream = NULL;

  l_stream = opj_stream_create_default_file_stream( l_file,true);

  this->m_Dinfo  = NULL;  /* handle to a decompressor */

  opj_image_t * l_image = NULL;

  /* decode the code-stream */
  /* ---------------------- */
  switch (m_DecompressionParameters.decod_format)
    {
    case Self::J2K_CFMT:
      {
      /* JPEG-2000 codestream */

      /* get a decoder handle */
      this->m_Dinfo = opj_create_decompress(CODEC_J2K);
      break;
      }
    case Self::JP2_CFMT:
      {
      /* JPEG 2000 compressed image data */
      /* get a decoder handle */
      this->m_Dinfo = opj_create_decompress(CODEC_JP2);
      break;
      }
    case Self::JPT_CFMT:
      {
      /* JPEG 2000, JPIP */
      /* get a decoder handle */
      this->m_Dinfo = opj_create_decompress(CODEC_JPT);
      break;
      }
    default:
      itkExceptionMacro("skipping file..\n");
      opj_stream_destroy(l_stream);
      return;
    }
  /* catch events using our callbacks and give a local context */

  /* setup the decoder decoding parameters using user parameters */
  opj_setup_decoder(this->m_Dinfo, &this->m_DecompressionParameters);

  OPJ_INT32 l_tile_x0,l_tile_y0;

  OPJ_UINT32 l_tile_width;
  OPJ_UINT32 l_tile_height;
  OPJ_UINT32 l_nb_tiles_x;
  OPJ_UINT32 l_nb_tiles_y;

  bool bResult = opj_read_header(
    this->m_Dinfo,
    & l_image,
    &l_tile_x0,
    &l_tile_y0,
    &l_tile_width,
    &l_tile_height,
    &l_nb_tiles_x,
    &l_nb_tiles_y,
    l_stream);

  if ( !bResult )
    {
    opj_destroy_codec(this->m_Dinfo);
    this->m_Dinfo = NULL;
    opj_stream_destroy( l_stream );
    fclose( l_file );
    itkExceptionMacro("ERROR opj_read_header failed");
    }

  ImageIORegion regionToRead = this->GetIORegion();

  ImageIORegion::SizeType  size  = regionToRead.GetSize();
  ImageIORegion::IndexType start = regionToRead.GetIndex();

  const unsigned int sizex = size[0];
  const unsigned int sizey = size[1];
  // const unsigned int sizez = size[2];

  const unsigned int startx = start[0];
  const unsigned int starty = start[1];
  // const unsigned int startz = start[2];

  OPJ_INT32 p_start_x = static_cast< OPJ_INT32 >( startx );
  OPJ_INT32 p_start_y = static_cast< OPJ_INT32 >( starty );
  OPJ_INT32 p_end_x   = static_cast< OPJ_INT32 >( startx + sizex );
  OPJ_INT32 p_end_y   = static_cast< OPJ_INT32 >( starty + sizey );

  std::cout << "opj_set_decode_area() before " << std::endl;
  std::cout << "p_start_x = " << p_start_x << std::endl;
  std::cout << "p_start_y = " << p_start_y << std::endl;
  std::cout << "p_end_x = " << p_end_x << std::endl;
  std::cout << "p_end_y = " << p_end_y << std::endl;

  bResult = opj_set_decode_area(
    this->m_Dinfo,
    p_start_x,
    p_start_y,
    p_end_x,
    p_end_y
    );

  std::cout << "opj_set_decode_area() after " << std::endl;

  if ( !bResult )
    {
    opj_destroy_codec(this->m_Dinfo);
    this->m_Dinfo = NULL;
    opj_stream_destroy( l_stream );
    fclose( l_file );
    itkExceptionMacro("ERROR opj_set_decode_area failed");
    }


  OPJ_INT32 l_current_tile_x0;
  OPJ_INT32 l_current_tile_y0;
  OPJ_INT32 l_current_tile_x1;
  OPJ_INT32 l_current_tile_y1;

  OPJ_UINT32 l_tile_index;
  OPJ_UINT32 l_data_size;

  OPJ_UINT32 l_nb_comps;

  OPJ_UINT32 l_max_data_size = 1000;

  bool l_go_on = true;

  OPJ_BYTE * l_data = (OPJ_BYTE *) malloc(1000);

  while ( l_go_on )
    {
    bool tileHeaderRead = opj_read_tile_header(
      this->m_Dinfo,
      &l_tile_index,
      &l_data_size,
      &l_current_tile_x0,
      &l_current_tile_y0,
      &l_current_tile_x1,
      &l_current_tile_y1,
      &l_nb_comps,
      &l_go_on,
      l_stream);

    if ( !tileHeaderRead )
      {
      free(l_data);
      opj_stream_destroy( l_stream );
      fclose( l_file );
      opj_destroy_codec( this->m_Dinfo );
      opj_image_destroy( l_image );
      itkExceptionMacro("Error opj_read_tile_header");
      }

    std::cout << "l_tile_index " << l_tile_index << std::endl;
    std::cout << "l_data_size " << l_data_size << std::endl;
    std::cout << "l_current_tile_x0 " << l_current_tile_x0 << std::endl;
    std::cout << "l_current_tile_y0 " << l_current_tile_y0 << std::endl;
    std::cout << "l_current_tile_x1 " << l_current_tile_x1 << std::endl;
    std::cout << "l_current_tile_y1 " << l_current_tile_y1 << std::endl;
    std::cout << "l_nb_comps " << l_nb_comps << std::endl;
    std::cout << "l_go_on " << l_go_on << std::endl;

    if ( l_go_on )
      {

      if ( l_data_size > l_max_data_size )
        {

        l_data = (OPJ_BYTE *) realloc( l_data, l_data_size );

        if ( ! l_data )
          {
          opj_stream_destroy( l_stream );
          fclose( l_file );
          opj_destroy_codec( this->m_Dinfo );
          opj_image_destroy( l_image );
          itkExceptionMacro("Error reallocating memory");
          }

        std::cout << "reallocated for " << l_data_size << std::endl;

        l_max_data_size = l_data_size;

        }

      bool decodeTileData = opj_decode_tile_data( 
        this->m_Dinfo,
        l_tile_index,
        l_data,
        l_data_size,
        l_stream);

      if ( ! decodeTileData )
        {
        free(l_data);
        opj_stream_destroy( l_stream );
        fclose( l_file );
        opj_destroy_codec( this->m_Dinfo );
        opj_image_destroy( l_image );
        itkExceptionMacro("Error opj_decode_tile_data");
        }

      std::cout << "decoding data size = " << l_data_size << std::endl;
      }
    }

  l_image = opj_decode( this->m_Dinfo, l_stream );

  if (! opj_end_decompress( this->m_Dinfo, l_stream ) )
    {
    free(l_data);
    opj_stream_destroy( l_stream );
    fclose( l_file );
    opj_destroy_codec( this->m_Dinfo );
    opj_image_destroy( l_image );
    itkExceptionMacro("ERROR opj_end_decompress");
    }

  if ( ! l_image )
    {
    opj_destroy_codec(this->m_Dinfo);
    this->m_Dinfo = NULL;
    opj_stream_destroy( l_stream );
    fclose( l_file );
    itkExceptionMacro("ERROR -> j2k_to_image: failed to decode image!");
    }

  const size_t numberOfPixels = sizex * sizey;

  size_t index = 0;

  // HERE, copy the buffer
  std::cout << "Copying " << numberOfPixels << " pixels " << std::endl;
  std::cout << "sizex " << sizex << " pixels " << std::endl;
  std::cout << "sizey " << sizey << " pixels " << std::endl;

  std::cout << "number of components = " << this->GetNumberOfComponents() << std::endl;


  std::cout << " START COPY BUFFER" << std::endl;

  OPJ_BYTE * l_data_ptr = l_data;

  const unsigned int numberOfComponents = this->GetNumberOfComponents();

  for ( unsigned int k = 0; k < numberOfComponents; k++)
    {
    unsigned char * charBuffer = (unsigned char *)buffer;
    charBuffer += k;

    for ( size_t j = 0; j < numberOfPixels; j++)
      {
      *charBuffer = (unsigned char)(*l_data_ptr++);
      charBuffer += numberOfComponents;
      //   l_image->comps[k].data[index];
      }
    index++;
    }

  std::cout << " END COPY BUFFER" << std::endl;

  /* close the byte stream */
  opj_stream_destroy( l_stream );
  fclose( l_file );

  if (this->m_Dinfo)
    {
    opj_destroy_codec(this->m_Dinfo);
    this->m_Dinfo = NULL;
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
  std::cout << "Write() " <<  this->GetNumberOfComponents() << std::endl;

  bool bSuccess;

  opj_cparameters_t parameters;
  opj_set_default_encoder_parameters(&parameters);

//--------------------------------------------------------
  // Copy the contents into the image structure
  int w, h;
  w = this->m_Dimensions[0];
  h = this->m_Dimensions[1];

  OPJ_COLOR_SPACE color_space = CLRSPC_GRAY;
  opj_image_cmptparm_t cmptparm[3];

  if ( this->GetNumberOfComponents() == 3 )
    {
    color_space = CLRSPC_SRGB;

    /* initialize image components */
    memset(&cmptparm[0], 0, 3*sizeof(opj_image_cmptparm_t));
    for ( unsigned int i = 0; i < 3; i++ )
      {
      cmptparm[i].prec = 8;
      cmptparm[i].bpp = 8;
      cmptparm[i].sgnd = 0;
      cmptparm[i].dx = 1;
      cmptparm[i].dy = 1;//this->GetSpacing( 1 )
      cmptparm[i].w =  w;
      cmptparm[i].h = h;
      }
    }
  else
    {
    color_space = CLRSPC_GRAY;

    /* initialize image components */
    memset(&cmptparm[0], 0, sizeof(opj_image_cmptparm_t));
    cmptparm[0].prec = 8;
    cmptparm[0].bpp = 8;
    cmptparm[0].sgnd = 0;
    cmptparm[0].dx = 1;
    cmptparm[0].dy = 1;//this->GetSpacing( 1 )
    cmptparm[0].w =  w;
    cmptparm[0].h = h;
    }


  opj_image_t * l_image = NULL;
  l_image = opj_image_create( this->GetNumberOfComponents(), &cmptparm[0], color_space);

  if( ! l_image )
    {
    itkExceptionMacro("Image buffer not created");
    }

  l_image->numcomps = this->GetNumberOfComponents();

  int subsampling_dx = parameters.subsampling_dx;
  int subsampling_dy = parameters.subsampling_dy;
  l_image->x0 = parameters.image_offset_x0;
  l_image->y0 = parameters.image_offset_y0;
  l_image->x1 = !l_image->x0 ? (w - 1) * subsampling_dx + 1 : l_image->x0 + (w - 1) * subsampling_dx + 1;
  l_image->y1 = !l_image->y0 ? (h - 1) * subsampling_dy + 1 : l_image->y0 + (h - 1) * subsampling_dy + 1;

  // HERE, copy the buffer
  unsigned char * charBuffer = (unsigned char *)buffer;
  size_t index = 0;
  size_t numberOfPixels = size_t(w) * size_t(h);
  std::cout << " START COPY BUFFER" << std::endl;
  for ( size_t j = 0; j < numberOfPixels; j++)
    {
    for ( unsigned int k = 0; k < this->GetNumberOfComponents(); k++)
      {
      l_image->comps[k].data[index] = *charBuffer++;
      }
    index++;
    }
  std::cout << " END COPY BUFFER" << std::endl;
//--------------------------------------------------------------------

  /* Create comment for codestream */
  if(parameters.cp_comment == NULL)
    {
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
    parameters.cod_format = Self::J2K_CFMT;
    }

  if( extension == ".jp2" )
    {
    cinfo = opj_create_compress(CODEC_JP2);
    parameters.cod_format = Self::JP2_CFMT;
    }

  if ( this->GetNumberOfComponents() == 3 )
    {
    parameters.tcp_mct = 1;
    }
  else
    {
    parameters.tcp_mct = 0;
    }

  /* if no rate entered, lossless by default */
  if (parameters.tcp_numlayers == 0)
    {
    parameters.tcp_rates[0] = 0; /* MOD antonin : losslessbug */
    parameters.tcp_numlayers++;
    parameters.cp_disto_alloc = 1;
    }

  if( (parameters.cp_tx0 > parameters.image_offset_x0) || (parameters.cp_ty0 > parameters.image_offset_y0))
    {
    itkExceptionMacro("Error: Tile offset dimension is unnappropriate -->"
      << "  TX0(" << parameters.cp_tx0 << ") <= IMG_X0( " << parameters.image_offset_x0
      << ") TYO(" << parameters.cp_ty0 << ") <= IMG_Y0( " << parameters.image_offset_y0 << ") ");
    return;
    }

  for ( int i = 0; i < parameters.numpocs; i++)
    {
    if (parameters.POC[i].prg == -1)
      {
      std::cerr << "Unrecognized progression order in option -P (POC n " << i+1;
      std::cerr << ") [LRCP, RLCP, RPCL, PCRL, CPRL] !!" << std::endl;
      }
    }

  opj_setup_encoder(cinfo, &parameters, l_image );

  FILE * l_file = NULL;
  l_file = fopen(parameters.outfile, "wb");
  if ( ! l_file )
    {
    itkExceptionMacro("failed to encode image");
    }

  /* open a byte stream for writing */
  /* allocate memory for all tiles */
  opj_stream_t *cio = 00;
  cio = opj_stream_create_default_file_stream(l_file,false);
  if (! cio)
    {
    itkExceptionMacro("no file stream opened");
    }

  /* encode the image */
  /*if (*indexfilename)         // If need to extract codestream information
    bSuccess = opj_encode_with_info(cinfo, cio, image, &cstr_info);
  else*/
  bSuccess = opj_start_compress(cinfo,l_image,cio);
  bSuccess = bSuccess && opj_encode(cinfo, cio);
  bSuccess = bSuccess && opj_end_compress(cinfo, cio);

  if (!bSuccess)
    {
    opj_stream_destroy(cio);
    fclose( l_file );
    itkExceptionMacro("failed to encode image");
    }

  /* close and free the byte stream */
  opj_stream_destroy(cio);
  fclose( l_file );

  //   /* Write the index to disk */
  //   if (*indexfilename)
  //   {
  //     bSuccess = write_index_file(&cstr_info, indexfilename);
  //     if (bSuccess)
  //       {
  //       itkExceptionMacro("Failed to output index file");
  //       }
  //   }

  /* free remaining compression structures */
  opj_destroy_codec(cinfo);
  //   if (*indexfilename)
  //     opj_destroy_cstr_info(&cstr_info);

  /* free image data */
  opj_image_destroy(l_image);
}


JPEG2000ImageIO::SizeType 
JPEG2000ImageIO::
GetHeaderSize(void ) const
{
  return 0;
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
