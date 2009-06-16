#include <stdio.h>

//
//  link to cudart.lib
//
//  This file should be compiled by the "nvcc" compiler.
//

#define SIZE 2048
#define DIVUP(a,b) ( a % b ) == 0 ? (a/b): (a/b) + 1;

__global__ void VectorAddKernel( float * Vector1, float * Vector2, float * Output, int size)
{
  int idx = blockIdx.x * blockDim.x + threadIdx.x;

  if( idx < size )
    {
    Output[idx] = Vector1[idx] + Vector2[idx];
    }
}

int main()
{

  float HostVector1[SIZE];
  float HostVector2[SIZE];
  float HostOutputVector[SIZE];


  for( int i=0; i<SIZE; i++)
    {
    HostVector1[i] = i;
    HostVector2[i] = i;
    }

  float * GPUVector1;
  float * GPUVector2;
  float * GPUOutputVector;

  cudaError_t err;

  unsigned int totalSize = SIZE* sizeof(float);

  err = cudaMalloc((void**)&GPUVector1, totalSize );
  err = cudaMalloc((void**)&GPUVector2, totalSize );
  err = cudaMalloc((void**)&GPUOutputVector, totalSize );

  cudaMemcpy(GPUVector1, HostVector1, totalSize , cudaMemcpyHostToDevice);
  cudaMemcpy(GPUVector2, HostVector2, totalSize , cudaMemcpyHostToDevice);

  //
  // Define here the strategy for defining the distribution of the problem
  //


  // Size of the data block that will be passed to each one of the streaming
  // multi-processors.
  dim3 BlockDim(128,1,1);

  // 
  // Size of the grid of multi-processors that will be used for processing
  // the total amount of data.
  //
  int numberOfProcessors = DIVUP(SIZE, BlockDim.x);
  dim3 GridDim( numberOfProcessors, 1 , 1 );
  // 17 blocks = 2050 / 128



  //
  // This call is asynchronous.
  //
  // Kernels have a timeout of 5 seconds... if the kernel runs for more than 5 seconds
  // The operating system (Microsoft Windows) will consider that the display crashed.
  //
  VectorAddKernel<<<GridDim,BlockDim>>>(GPUVector1,GPUVector2,GPUOutputVector,SIZE);


  //
  // Do other stuff here...
  // 


  //
  // This call will wait until the GPU is done
  // cudaThreadSynchronize();
  //

  err = cudaMemcpy( HostOutputVector, GPUOutputVector, totalSize, cudaMemcpyDeviceToHost);

  err = cudaFree( GPUVector1 );  
  err = cudaFree( GPUVector2 );  
  err = cudaFree( GPUOutputVector );  

  for(int i=0; i<SIZE; i++)
    {
    printf("%8.3f\n",HostOutputVector[i]);
    }

  if( err )
    {
    printf("err %d", err );
    }
}
