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



// We are going to use texture fetching to access the source image.  This allows for effecient
// non-uniform access to image memory and includes hardware bilinear interpolation.  
// The data type, dimension, and read-mode are compile-time set paramters
texture<uchar4, 2, cudaReadModeNormalizedFloat> ImageTex;


// Here is the kernel which performs the rotate for every pixel in the output image
__global__ void RotateKernel(uchar4 * out_image, int w, int h, int pitch, float angle)
{
  int x = __mul24(blockIdx.x,blockDim.x) + threadIdx.x;
  int y = __mul24(blockIdx.y,blockDim.y) + threadIdx.y;
   
  if(x < w && y < h)
  {
    // variables to hold the center coordinate of the image.
      // Texture operations use floating point coordinates
    float xf = (float)x - (float)w / 2.0f;
    float yf = (float)y - (float)h / 2.0f;

    // transform to polar coordinates
    float r = sqrtf((float)(xf*xf + yf*yf));
    float theta = atan2f(xf,yf) + 3.14159f / 2.0f;
      
    // compute the new theta by adding the rotation angle
    theta += angle * (3.14159f / 180.0f);

    

    // convert back to cartesian coordinates oriented at 0,0
    yf = r * sinf(theta);   
    yf += (float)h / 2.0f;
    
    xf = -r * cosf(theta);       
    xf += (float)w / 2.0f;  

    // perform the texture lookup to retrieve the new value.
    // It will be returned as a 0 -> 1.0 normalized float
    // because we are doing hardware bilinear interpolation
    float4 out_val_f = tex2D(ImageTex,xf,yf);
    
    // convert back to unsigned char and scale.
    uchar4 out_val;
    out_val.x = (unsigned char)(255.0f * out_val_f.x);
    out_val.y = (unsigned char)(255.0f * out_val_f.y);
    out_val.z = (unsigned char)(255.0f * out_val_f.z);
    out_val.w = 255;
    
    // write the uchar4 value to GMEM
    out_image[__mul24(y,pitch) + x] = out_val;
  }

}

// the Function doRotate is a host function which invokes the kernel to execute the rotate
extern "C" cudaError_t RotateImage(cudaArray * imageArray, uchar4 * out_image, int width, int height, size_t out_pitch, float angle)
{

  cudaError_t c_err;

  // We want to use the hardware bilinear interpolation
  // for improved quality when we modify the image.  
  // This is enabled by setting the runtime filterMode 
  // parameter of the texture.
  ImageTex.filterMode = cudaFilterModeLinear;
  ImageTex.addressMode[0] = cudaAddressModeClamp;
  ImageTex.addressMode[1] = cudaAddressModeClamp;

  // The first step is to bind the cudaArray countaining
  // the source image to the texture.
  c_err = cudaBindTextureToArray(ImageTex, imageArray);
  if(c_err != cudaSuccess) return c_err;
  // we need to create a 2-d thread block and a 2-d grid
  // of blocks.  Lets just make the blocks 16x16.   
  dim3 BlockSz(16,16,1);
  dim3 GridSz(DIVUP(width,BlockSz.x),DIVUP(height,BlockSz.y),1);
  
  // Now launch the kernel to do the rotation
  RotateKernel<<<GridSz,BlockSz>>>(out_image,width,height,(int)out_pitch / 4,angle);
  c_err = cudaGetLastError();
  if(c_err != cudaSuccess) return c_err;


  c_err = cudaThreadSynchronize();

  // Unbind the texture
  cudaUnbindTexture(ImageTex);

      
  return(c_err);
}



texture<unsigned char, 1, cudaReadModeElementType> GBR_Tex;

__global__ void convertBGR_U8_to_RGBA_U8_kernel(uchar4 * dest, int width, int height, int dest_pitch_32b)
{
    // compute the X & Y coordinates of the first pixel this thread will process
    int X = __mul24(blockIdx.x,blockDim.x) + threadIdx.x;
    int Y = __mul24(blockIdx.y,blockDim.y) + threadIdx.y;
    
    if(X < width && Y < height)
  {
        uchar4 out_val;
                
        int pix_idx = __mul24(Y,__mul24(width,3)) + __mul24(X,3);

    out_val.z = tex1Dfetch(GBR_Tex,pix_idx);
    out_val.y = tex1Dfetch(GBR_Tex,pix_idx+1);
    out_val.x = tex1Dfetch(GBR_Tex,pix_idx+2);
    out_val.w = 255;
       
        // write the result to GMEM
        dest[__mul24(Y,dest_pitch_32b) + X] = out_val;
    }
}



extern "C" cudaError_t convertBGRtoRGBA(unsigned char * pGPUtemp,uchar4 * pGPUImage, int width, int height, size_t out_pitch)
{
  cudaChannelFormatDesc uchar_desc = cudaCreateChannelDesc<unsigned char>();
  cudaBindTexture(NULL,GBR_Tex,pGPUtemp,uchar_desc);

  dim3 BlockSz(16,16,1);
  dim3 GridSz(DIVUP(width,BlockSz.x),DIVUP(height,BlockSz.y),1);

  convertBGR_U8_to_RGBA_U8_kernel<<<GridSz,BlockSz>>>(pGPUImage, width, height, (int)out_pitch / 4);

  cudaUnbindTexture(GBR_Tex);

  return(cudaGetLastError());

}
