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

extern "C" {
  #include "openjpeg/openjpeg.h"
  #include "openjpeg/j2k.h"
  #include "openjpeg/jp2.h"
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

/**
sample error callback expecting a FILE* client object
*/
extern "C" {
void openjpeg_error_callback(const char *msg, void *) {
  std::cerr << "Error in JPEG2000ImageIO" << msg << std::endl;
}
/**
sample warning callback expecting a FILE* client object
*/
void openjpeg_warning_callback(const char *msg, void *) {
  std::cerr << "Warning in JPEG2000ImageIO" << msg << std::endl;
}
/**
sample debug callback expecting no client object
*/
void openjpeg_info_callback(const char * msg, void *) {
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

void open_j2k_dump_cp(FILE *fd, opj_image_t * img, opj_cp_t * cp) {
  int tileno, compno, layno, bandno, resno, numbands;
  fprintf(fd, "coding parameters {\n");
  fprintf(fd, "  tx0=%d, ty0=%d\n", cp->tx0, cp->ty0);
  fprintf(fd, "  tdx=%d, tdy=%d\n", cp->tdx, cp->tdy);
  fprintf(fd, "  tw=%d, th=%d\n", cp->tw, cp->th);
  for (tileno = 0; tileno < cp->tw * cp->th; tileno++) {
    opj_tcp_t *tcp = &cp->tcps[tileno];
    fprintf(fd, "  tile %d {\n", tileno);
    fprintf(fd, "    csty=%x\n", tcp->csty);
    fprintf(fd, "    prg=%d\n", tcp->prg);
    fprintf(fd, "    numlayers=%d\n", tcp->numlayers);
    fprintf(fd, "    mct=%d\n", tcp->mct);
    fprintf(fd, "    rates=");
    for (layno = 0; layno < tcp->numlayers; layno++) {
      fprintf(fd, "%d ", tcp->rates[layno]);
    }
    fprintf(fd, "\n");
    for (compno = 0; compno < img->numcomps; compno++) {
      opj_tccp_t *tccp = &tcp->tccps[compno];
      fprintf(fd, "    comp %d {\n", compno);
      fprintf(fd, "      csty=%x\n", tccp->csty);
      fprintf(fd, "      numresolutions=%d\n", tccp->numresolutions);
      fprintf(fd, "      cblkw=%d\n", tccp->cblkw);
      fprintf(fd, "      cblkh=%d\n", tccp->cblkh);
      fprintf(fd, "      cblksty=%x\n", tccp->cblksty);
      fprintf(fd, "      qmfbid=%d\n", tccp->qmfbid);
      fprintf(fd, "      qntsty=%d\n", tccp->qntsty);
      fprintf(fd, "      numgbits=%d\n", tccp->numgbits);
      fprintf(fd, "      roishift=%d\n", tccp->roishift);
      fprintf(fd, "      stepsizes=");
      numbands = tccp->qntsty == J2K_CCP_QNTSTY_SIQNT ? 1 : tccp->numresolutions * 3 - 2;
      for (bandno = 0; bandno < numbands; bandno++) {
        fprintf(fd, "(%d,%d) ", tccp->stepsizes[bandno].mant,
          tccp->stepsizes[bandno].expn);
      }
      fprintf(fd, "\n");
      
      if (tccp->csty & J2K_CCP_CSTY_PRT) {
        fprintf(fd, "      prcw=");
        for (resno = 0; resno < tccp->numresolutions; resno++) {
          fprintf(fd, "%d ", tccp->prcw[resno]);
        }
        fprintf(fd, "\n");
        fprintf(fd, "      prch=");
        for (resno = 0; resno < tccp->numresolutions; resno++) {
          fprintf(fd, "%d ", tccp->prch[resno]);
        }
        fprintf(fd, "\n");
      }
      fprintf(fd, "    }\n");
    }
    fprintf(fd, "  }\n");
  }
  fprintf(fd, "}\n");
}



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
  // JPEG2000 By NOW.... only reads 8-bits unsigned char images. (FIXME)
  this->SetPixelType( SCALAR );
  this->SetComponentType( UCHAR );

  this->m_InputStream.open( this->m_FileName.c_str() );
 
   opj_dparameters_t parameters;  /* decompression parameters */
  opj_event_mgr_t event_mgr;    /* event manager */
  opj_image_t *image = NULL;
  FILE *fsrc = NULL;
  unsigned char *src = NULL; 
  int file_length;

  opj_dinfo_t* dinfo = NULL;  /* handle to a decompressor */
  opj_cio_t *cio = NULL;

  /* configure the event callbacks (not required) */
  memset(&event_mgr, 0, sizeof(opj_event_mgr_t));
  event_mgr.error_handler = openjpeg_error_callback;
  event_mgr.warning_handler = openjpeg_warning_callback;
  event_mgr.info_handler = openjpeg_info_callback;

  /* set decoding parameters to default values */
  opj_set_default_decoder_parameters(&parameters);

  /* read the input file and put it in memory */
  /* ---------------------------------------- */
  fsrc = fopen( this->m_FileName.c_str(), "rb");
  if (!fsrc)
    {
    itkExceptionMacro("Failed to open" << this->m_FileName );
    }  

  fseek(fsrc, 0, SEEK_END);
  file_length = ftell(fsrc);
  fseek(fsrc, 0, SEEK_SET);
  src = (unsigned char *) malloc(file_length);
  size_t fer = fread(src, 1, file_length, fsrc);

  if( fer == 0 )
    {
    fprintf(stderr,"error while reading");
    }

  fclose(fsrc);
  
  switch(parameters.decod_format) 
    {
    case J2K_CFMT:
      {
      /* JPEG-2000 codestream */

      /* get a decoder handle */
      dinfo = opj_create_decompress(CODEC_J2K);
      
      /* catch events using our callbacks and give a local context */
      opj_set_event_mgr((opj_common_ptr)dinfo, &event_mgr, stderr);      

      /* setup the decoder decoding parameters using user parameters */
      opj_setup_decoder(dinfo, &parameters);

      /* open a byte stream */
      cio = opj_cio_open((opj_common_ptr)dinfo, src, file_length);

      /* decode the stream and fill the image structure */
      image = opj_decode(dinfo, cio);
      if(!image) {
        opj_destroy_decompress(dinfo);
        opj_cio_close(cio);
        itkExceptionMacro( "ERROR failed to decode JPEG2000 image");
      }
      
      /* close the byte stream */
      opj_cio_close(cio);
      }
      break;

    case JP2_CFMT:
      {
      /* JPEG 2000 compressed image data */

      /* get a decoder handle */
      dinfo = opj_create_decompress(CODEC_JP2);
      
      /* catch events using our callbacks and give a local context */
      opj_set_event_mgr((opj_common_ptr)dinfo, &event_mgr, stderr);      

      /* setup the decoder decoding parameters using the current image and using user parameters */
      opj_setup_decoder(dinfo, &parameters);

      /* open a byte stream */
      cio = opj_cio_open((opj_common_ptr)dinfo, src, file_length);

      /* decode the stream and fill the image structure */
      image = opj_decode(dinfo, cio);
      if(!image)
        {
        opj_destroy_decompress(dinfo);
        opj_cio_close(cio);
        itkExceptionMacro("ERROR: failed to decode JPEG2000 image");
        }

      /* close the byte stream */
      opj_cio_close(cio);

      }
      break;

    case JPT_CFMT:
      {
      /* JPEG 2000, JPIP */

      /* get a decoder handle */
      dinfo = opj_create_decompress(CODEC_JPT);
      
      /* catch events using our callbacks and give a local context */
      opj_set_event_mgr((opj_common_ptr)dinfo, &event_mgr, stderr);      

      /* setup the decoder decoding parameters using user parameters */
      opj_setup_decoder(dinfo, &parameters);

      /* open a byte stream */
      cio = opj_cio_open((opj_common_ptr)dinfo, src, file_length);

      /* decode the stream and fill the image structure */
      image = opj_decode(dinfo, cio);
      if(!image)
        {
        opj_destroy_decompress(dinfo);
        opj_cio_close(cio);
        itkExceptionMacro("ERROR: failed to decode JPEG2000 image");        
        }  

      /* close the byte stream */
      opj_cio_close(cio);
      }
      break;

    default:
      itkExceptionMacro("Unknown input image format");
    }

  /* free the memory containing the code-stream */
  free(src);
  src = NULL;

  {
  opj_j2k_t* j2k = NULL;
  opj_jp2_t* jp2 = NULL;

  j2k = (opj_j2k_t*)dinfo->j2k_handle;
  jp2 = (opj_jp2_t*)dinfo->jp2_handle;

  if( j2k )
    {
    open_j2k_dump_cp(stdout, image, j2k->cp );
    }

  if( jp2 )
    {
    open_j2k_dump_cp(stdout, image, jp2->j2k->cp );
    }
  }

  /* create output image */
  /* ------------------- */

  switch (parameters.cod_format) {
    case PXM_DFMT:      /* PNM PGM PPM */
      imagetopnm(image, parameters.outfile);
      break;
            
    case PGX_DFMT:      /* PGX */
      imagetopgx(image, parameters.outfile);
      break;
    
    case BMP_DFMT:      /* BMP */
      imagetobmp(image, parameters.outfile);
      break;
  }

  /* free remaining structures */
  if(dinfo) {
    opj_destroy_decompress(dinfo);
  }

  /* free image data structure */
  opj_image_destroy(image);
     
//  this->SetDimensions( 0, nx );
//  this->SetDimensions( 1, ny );

//  this->SetSpacing( 0, dx );
//  this->SetSpacing( 1, dy );
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
