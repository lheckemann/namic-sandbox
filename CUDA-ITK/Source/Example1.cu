
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

  err = cudaMalloc((void**)&GPUVector1, SIZE*sizeof(float));
  err = cudaMalloc((void**)&GPUVector2, SIZE*sizeof(float));

}
