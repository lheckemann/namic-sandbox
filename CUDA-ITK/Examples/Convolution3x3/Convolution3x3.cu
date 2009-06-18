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

// Declare constant memory for our convolution filter
__constant__ float FilterCoeff[9];

// Here's the CPU data for our filter which we'll copy into the constant memory
float FilterCoeff_CPU[9] = { -1.0f, -2.0f, -1.0f,
               -2.0f, 12.0f, -2.0f,
               -1.0f, -2.0f, -1.0f};



#define BLOCK_W 16
#define BLOCK_H 16
#define TILE_W (BLOCK_W * 4 + 16)  // need 8 pixel aprons because we are doing 64-bit reads)
#define TILE_H (BLOCK_H + 2)


__global__ void Convolution3x3Kernel(uchar4 * in_image, uchar4 * out_image, int width, int height, int pitch32)
{

  // This is a pointer to the shared memory we use for the image tile
  __shared__ unsigned char smem[TILE_W*TILE_H];
  int2 * smem_64bit = (int2*)smem;

  // Compute the thread index overall
  int X = __mul24(blockDim.x,blockIdx.x) + threadIdx.x;  
  int Y = __mul24(blockDim.y,blockIdx.y) + threadIdx.y;
  
  // variables to hold the addresses we are going to write & read in GMEM & SMEM
  int smem_idx;
  int gmem_idx;

  // Perform 64-bit reads, so only some of the threads need to participate.  
  // we'll cast the source image to a 64-bit data type (e.g. int2) and them read into a the smem with the same
  // 64-bit casting
  
  if(threadIdx.x < BLOCK_W / 2 + 2)
  {
    int row = CLAMP(Y-1,0,height-1);
    int col = __mul24(blockDim.x>>1,blockIdx.x) + threadIdx.x - 1;  // reading 8 bytes / thread, but only 1/2 of the block is used so divide blockDim / 2
    col = CLAMP(col,0,width>>3-1);
    gmem_idx = __mul24(row,pitch32>>1) + col;            // must divide the pitch32 & X by 2 since it's a 64-bit address
    smem_idx = __mul24(threadIdx.y,TILE_W/8) + threadIdx.x;        // the index into the 64-bit casted smem
  
    // Perform the read
    smem_64bit[smem_idx] = ((int2*)in_image)[gmem_idx];

    // two rows need to read again for the lower apron rows
    if(threadIdx.y < 2)
    {
      row = CLAMP(Y + BLOCK_H - 1,0,height-1);      
      gmem_idx = __mul24(row,pitch32>>1) + col;  
      smem_idx += TILE_W/8 * BLOCK_H;
      smem_64bit[smem_idx] = ((int2*)in_image)[gmem_idx];
    }
  }
    

  // Now we need to wait until all threads in this block have finished reading
  // their respective pixels into SMEM
  __syncthreads();
  if(X < width && Y < height) 
  {
    // Now for the convolutions.  
    uchar4 out;  // variable to store our output
    unsigned int f_idx = 0;  // index into the filter coefficients
    float sum;  

    // compute the location of the pixel in smem we are going to start to process.  
    // this will be the pixel to the top-left of the active pixel
    smem_idx = __mul24(threadIdx.y,TILE_W) + (threadIdx.x<<2) +7;

    // now do the math.  We'll use floating point since it's fastest
    // Pixel 1
    sum = FilterCoeff[f_idx++] * (float)smem[smem_idx];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+1];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+2];
    smem_idx += TILE_W;
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+1];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+2];
    smem_idx += TILE_W;
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+1];
    sum += FilterCoeff[f_idx] * (float)smem[smem_idx+2];
    out.x = (unsigned char)CLAMP(sum,0,255.0f);

    // Pixel 2
    f_idx = 0;
    smem_idx = __mul24(threadIdx.y,TILE_W) + (threadIdx.x<<2) +8;
    sum = FilterCoeff[f_idx++] * (float)smem[smem_idx];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+1];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+2];
    smem_idx += TILE_W;
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+1];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+2];
    smem_idx += TILE_W;
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+1];
    sum += FilterCoeff[f_idx] * (float)smem[smem_idx+2];
    out.y = (unsigned char)CLAMP(sum,0,255.0f);

    // Pixel 3
    f_idx = 0;
    smem_idx = __mul24(threadIdx.y,TILE_W) + (threadIdx.x<<2) + 9;
    sum = FilterCoeff[f_idx++] * (float)smem[smem_idx];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+1];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+2];
    smem_idx += TILE_W;
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+1];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+2];
    smem_idx += TILE_W;
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+1];
    sum += FilterCoeff[f_idx] * (float)smem[smem_idx+2];
    out.z = (unsigned char)CLAMP(sum,0,255.0f);

    // Pixel 4
    f_idx = 0;
    smem_idx = __mul24(threadIdx.y,TILE_W) + (threadIdx.x<<2) +10;
    sum = FilterCoeff[f_idx++] * (float)smem[smem_idx];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+1];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+2];
    smem_idx += TILE_W;
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+1];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+2];
    smem_idx += TILE_W;
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx];
    sum += FilterCoeff[f_idx++] * (float)smem[smem_idx+1];
    sum += FilterCoeff[f_idx] * (float)smem[smem_idx+2];
    out.w = (unsigned char)CLAMP(sum,0,255.0f);

    // **DEBUG**
    // The following lines of code are useful for debugging 
    // They simply copy the value of the primary pixels back to the output rather
    // than performing the convolution.  Thus, it's easy to tell if the GMEM is loaded
    // into SMEM properly. It's also useful to shift the offsets to insure the aprons are correct
    //out.x = smem[(threadIdx.y+1)*TILE_W + threadIdx.x*4+7];
    //out.y = smem[(threadIdx.y+1)*TILE_W + threadIdx.x*4+8];
    //out.z = smem[(threadIdx.y+1)*TILE_W + threadIdx.x*4+9];
    //out.w = smem[(threadIdx.y+1)*TILE_W + threadIdx.x*4+10];

    
    // Finally, write out the result!    
    gmem_idx = __mul24(Y,pitch32) + X;
    out_image[gmem_idx] = out;
  }
}



// Function to preform an arbitrary 3x3 convolution on a grayscale 8-bit image
extern "C" cudaError_t Convolution3x3(unsigned char * src, unsigned char * dest, int width, int height, size_t pitch)
{
  cudaError_t cerr;

  // First, copy data for the filter coefficients into constant memory
  cerr = cudaMemcpyToSymbol(FilterCoeff,FilterCoeff_CPU,9*sizeof(float));
  if(cerr != cudaSuccess) return(cerr);

  // Compute our block & grid dimensions
  dim3 BlockSz(BLOCK_W,BLOCK_H,1);
  dim3 GridSz(DIVUP(width,BlockSz.x*4),DIVUP(height,BlockSz.y),1);
  
  
  Convolution3x3Kernel<<<GridSz,BlockSz>>>((uchar4*)src, (uchar4*)dest,
                            width, height, (int)pitch/4);

  // Wait for this function to complete before we return
  cerr = cudaThreadSynchronize();

  return(cerr);  

}
