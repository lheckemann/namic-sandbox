
#define SIZE 2048

__global__ void VectorAddKernel( float * Vector1, float * Vector2, float * Octput)
{
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  Output[idx] = Vector1[idx] + Vector2[idx];
}
