//
//  link to cudart.lib
//
//  This file should be compiled by the "nvcc" compiler.
//

#define SIZE 2048

__global__ void VectorAddKernel( float * Vector1, float * Vector2, float * Octput)
{
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  Output[idx] = Vector1[idx] + Vector2[idx];
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


  dim3 BlockDim(128,1,1);
  dim3 GidDim(DIVUP(SIZE, BlockDim.x),1,1);

  VectorAddKernel<<<GridDim,BlockDim>>(GPUVector1,GPUVector2,GPUOutputVector,SIZE);

  err = cudaMemory( HostOutputVector, GPUOutputVector, 

  err = cudaFree( GPUVector1 );  
  err = cudaFree( GPUVector2 );  
  err = cudaFree( GPUOutputVector );  

  for(int i=0; i<SIZE; i++)
    {
    printf("%8.3f\n",HostOutputVector[i]);
    }
}
