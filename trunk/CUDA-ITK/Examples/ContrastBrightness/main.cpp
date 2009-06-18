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


#pragma warning(disable : 4996) // disable warnings with stdio
#include <cuda_runtime.h>
#include "ImageUtils.h"
#include <stdio.h>

#ifdef WIN32
#include <conio.h>
#include "SimpleCudaGLWindow.h"
#endif

// Declaration of the AdjustBrightnessContract function in "BrightnessContrast.cu"
extern "C" cudaError_t AdjustBrightnessContrast(unsigned char * image, int width, int height, 
                          int pitch, float brightness_adjust, float contrast_adjust);

  // The main function
int main(int argc, char ** argv)
{
  // insure a filename was specified on the command line.
  if(argc < 2)  
  {
    printf("You must enter a .bmp file to load.\nPress Enter to Exit...\n");
    getchar();
    return(-1);
  }
  
  unsigned char * p_CPUImage;    // a pointer to the raw image data in CPU memory
  int width;        // width of the image, in pixels
  int height;    // height of the image (rows)
  int channels;    // number of color channels (1 = greyscale)

  // load the image using the BMP loader file specified in ImageUtil.h
  p_CPUImage = LoadBMPImage(argv[1],&width, &height, &channels);
  if(!p_CPUImage)
    {
    printf("Unable to load image\nPress Enter to Exit...\n");
    getchar();
    return(-1);
  }

  if(channels != 1)
  {
    printf("This example requires an 8-bit grayscale image.");
    getchar();
    return(-1);
  }

  

  // Allocate memory on the GPU for the image.  We'll use cudaMallocPitch, to pad the width if needed
  // to ensure that column 0 is always on a 64-byte boundary
  unsigned char * p_GPUimage;    // a pointer for the location of the image in GPU memory.  We'll
  size_t pitch;    // pitch is the number of bytes between each row
  cudaMallocPitch((void**)&p_GPUimage,&pitch,width,height);

  // Now we'll copy the image data to the GPU. We'll use cudaMemcpy2D because the pitch may be differen than the width
  cudaMemcpy2D(p_GPUimage,pitch,p_CPUImage,width,width,height,cudaMemcpyHostToDevice);

  

  // Parameters for the brightness & contrast adjustments.
  float brightness = 10.0f;
  float contrast = 0.7f;
  printf("\nEnter Brightness Adjust Factor +/-: ");
  scanf("%f",&brightness);
  printf("\nEnter Contrast Adjust Factor: ");
  scanf("%f",&contrast);
  
  printf("\n Drawing Original Image...");

#ifdef WIN32
  // Create an OpenGL window to display the image.  
  SimpleCudaGLWindow wnd;
  wnd.CreateCudaGLWindow(50,50,640,480,"Brightness Contrast Adjust");
  wnd.Flip = true;

  // Lets display the image before the conversion
  wnd.DrawImage(p_GPUimage,width,height,pitch,GL_LUMINANCE,GL_UNSIGNED_BYTE);

  printf("\nPress ENTER to perform conversion\n");
  while(!_kbhit())
    {
    wnd.CheckWindowsMessages();
    }

  _getch();
#endif

  // Now call the function in ContrastBrightness.cu which performs the adjustment on the GPU
  AdjustBrightnessContrast(p_GPUimage, width, height, (int)pitch, brightness, contrast);

#ifdef WIN32
  // Draw the output image
  wnd.DrawImage(p_GPUimage,width,height,pitch,GL_LUMINANCE,GL_UNSIGNED_BYTE);
  
  printf("\nPress Enter to Close...\n");
  while(!_kbhit())
    {
    wnd.CheckWindowsMessages();
    }
#endif

  // Free the CPU and GPU memory
  //int * s = (int*)malloc(10);
  delete p_CPUImage;
  cudaFree(p_GPUimage);
  
  return(0);

}
