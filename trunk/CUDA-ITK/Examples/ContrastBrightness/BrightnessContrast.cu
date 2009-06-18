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


// A simple macro to divide and round-up
#define DIVUP(A,B) ( (A)%(B) == 0 ? (A)/(B) : ((A) / (B) + 1) )

// macro to clamp to min & max value:
#define CLAMP(A,B,C) ( (A) < (B) ? (B) : (A) > (C) ? (C) : (A) )


// Here is the kernel which performs the rotate for every pixel in the output image
__global__ void BrightnessContrastKernel(uchar4 * image, int w, int h, int pitch, float brightness, float contrast)
{
  // compute the x & y coordinates in the image the current pixel must process.  

  // the image data is passed in as a uchar4, so this x coordinate 
  // points 4-pixel groups
  int x = __mul24(blockIdx.x,blockDim.x) + threadIdx.x; 
  int y = __mul24(blockIdx.y,blockDim.y) + threadIdx.y;
   
  if(x < (w>>2) && y < h)    // w/4 because we are processing 4 pixels per thread
  {

    float temp;

    // read in the value to modify
    uchar4 pixel = image[__umul24(y,pitch>>2) + x];

    // Pixel 1 
    temp = ((float)pixel.x + brightness) * contrast;
    pixel.x = CLAMP(temp,0.0f,255.0f);

    // Pixel 2 
    temp = ((float)pixel.y + brightness) * contrast;
    pixel.y = CLAMP(temp,0.0f,255.0f);

    // Pixel 3
    temp = ((float)pixel.z + brightness) * contrast;
    pixel.z = CLAMP(temp,0.0f,255.0f);

    // Pixel 4
    temp = ((float)pixel.w + brightness) * contrast;
    pixel.w = CLAMP(temp,0.0f,255.0f);

    // write the new pixel value back to the image data
    image[__umul24(y,pitch>>2) + x] = pixel;
  }

}

// This function will adjust the brightness and contrast an image
extern "C" cudaError_t AdjustBrightnessContrast(unsigned char * image, int width, int height, 
                          int pitch, float brightness_adjust, float contrast_adjust)
{
  
  // we need to create a 2-d thread block and a 2-d grid
  // of blocks.  Lets just make the blocks 16x14. 
  // Process four pixels / thread
  dim3 BlockSz(16,14,1);
  dim3 GridSz(DIVUP(width,BlockSz.x*4),DIVUP(height,BlockSz.y),1);

  // Now launch the kernel to do the adjustment
  BrightnessContrastKernel<<<GridSz,BlockSz>>>((uchar4*)image,width,height,pitch,brightness_adjust,contrast_adjust);
  
  // just wait here for the kernel to complete
  cudaError_t err = cudaThreadSynchronize();  

  return(err);
}


