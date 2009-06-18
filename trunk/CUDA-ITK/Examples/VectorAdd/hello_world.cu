




#include <stdio.h>

#define SIZE 2050
#define DIVUP(a,b) (a % b) == 0 ? (a / b) : (a / b) + 1


__global__ void VectorAddKernel(float * Vector1, float * Vector2, float * Output, int size)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if(idx < size)
        Output[idx] = Vector1[idx] + Vector2[idx];
}


int main()
{
    float HostVector1[SIZE];
    float HostVector2[SIZE];
    float HostOutputVector[SIZE];

    for(int i=0;i<SIZE;i++)
    {
        HostVector1[i] = i;
        HostVector2[i] = i;
    }

    float * GPUVector1;
    float * GPUVector2;
    float * GPUOutputVector;

    cudaError_t err;
    err = cudaMalloc((void**)&GPUVector1,SIZE*sizeof(float));
    err = cudaMalloc((void**)&GPUVector2,SIZE*sizeof(float));
    err = cudaMalloc((void**)&GPUOutputVector,SIZE*sizeof(float));

    err = cudaMemcpy(GPUVector1,HostVector1,SIZE*sizeof(float),cudaMemcpyHostToDevice);
    err = cudaMemcpy(GPUVector2,HostVector2,SIZE*sizeof(float),cudaMemcpyHostToDevice);

    dim3 BlockDim(64,1,1);
    dim3 GridDim(DIVUP(SIZE,BlockDim.x),1,1);

    VectorAddKernel<<<GridDim,BlockDim>>>(GPUVector1,GPUVector2,GPUOutputVector,SIZE);
    
    // Do other stuff...

    cudaThreadSynchronize();

    err = cudaMemcpy(HostOutputVector,GPUOutputVector,SIZE*sizeof(float),cudaMemcpyDeviceToHost);
    
    err = cudaFree(GPUVector1);
    err = cudaFree(GPUVector2);
    err = cudaFree(GPUOutputVector);

    for(int i=0;i<SIZE; i++)
        printf("%8.3f\n",HostOutputVector[i]);



}
