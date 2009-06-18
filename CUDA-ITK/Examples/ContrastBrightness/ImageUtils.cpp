/*
 * Copyright 1993-2009 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

#include "ImageUtils.h"

#pragma warning(disable : 4996) // disable warnings with stdio
#include <stdio.h>



// ImageUtils.c
// Utility code to support image processing tutorial examples

/////////////////////
// LoadBMPImage
// 
// A simple function to load a windows .bmp image file.  This supports standard, uncompressed 
// 8-bit monochrome or GBR 8-8-8 bit color images.  The function allocates standard CPU memory
// to hold the image and returns a pointer to this memory (the calling code must free the data)
// This is a very basic implementation of a bitmap image loader, and is not compatible with all 
// possible valid bitmap files

unsigned char * LoadBMPImage(char * filename, int * width, int * height, int * channels)
{
  // First, some structures needed for loading a BMP
  #pragma pack(1)
  // The bitmap header
  struct {
    short type;
    int size;
    short reserved1;
    short reserved2;
    int offset;
  } hdr;

  // The bitmap info header
  struct {
    int size;
    int width;
    int height;
    short planes;
    short bitsPerPixel;
    unsigned compression;
    unsigned imageSize;
    int xPelsPerMeter;
    int yPelsPerMeter;
    int clrUsed;
    int clrImportant;
  } infoHdr;

  FILE * fd;
  // open the file and read the properties
    if( !(fd = fopen(filename,"rb")) )  return(NULL); 
   
    fread(&hdr, sizeof(hdr), 1, fd);
    if(hdr.type != 0x4D42) return(NULL);  // Wrong type

    fread(&infoHdr, sizeof(infoHdr), 1, fd);

  if(infoHdr.bitsPerPixel != 24 && infoHdr.bitsPerPixel != 8)
  {
    fclose(fd);    
    return NULL; // unsupported color type
  }

    if(infoHdr.compression)
  {
    fclose(fd);
    return NULL; // don't support compressed bitmpas
  }

  // set the color and size variables
  *channels = infoHdr.bitsPerPixel / 8;
  *width = infoHdr.width;
  *height = infoHdr.height;

  int size;
  size = *width * *height * *channels;

  // Allocate CPU memory
  unsigned char * pImage = new unsigned char[size];
  
  // Now read in the pixels from the file
    fseek(fd, hdr.offset - sizeof(hdr) - sizeof(infoHdr), SEEK_CUR);

  // read in the image all at once
  if(fread(pImage,1,size,fd) != size)
  {
    // incorrect # of bytes read
    delete pImage;
    fclose(fd);
    return(NULL);
  }

    if(ferror(fd))
  {    
    // Some other type of file error
    delete pImage;
    fclose(fd);
    return(NULL);
  }

  fclose(fd);

  // All OK, return the image pointer
  return (pImage);
  

}
