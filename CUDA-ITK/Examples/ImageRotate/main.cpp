#include <stdio.h>

#include "ImageUtils.h"

#ifdef WIN32
#include <conio.h>
#include "SimpleCudaGLWindow.h"
#endif

extern "C" cudaError_t RotateImage(cudaArray * imageArray, uchar4 * out_image, int width, int height, size_t out_pitch, float angle);
extern "C" cudaError_t convertBGRtoRGBA(unsigned char * pGPUtemp,uchar4 * pGPUImage, int width, int height, size_t out_pitch);
  // The main function

int main(int argc, char ** argv)
{
  cudaError_t c_err;
  // insure a filename was specified on the command line.
  if(argc < 2)  
  {
    printf("You must enter a .bmp file to load.\nPress Enter to Exit...\n");
    getchar();
    return(-1);
  }
  
  unsigned char * p_CPUImageGBR;    // a pointer to the raw image data in CPU memory
  int width;            // width of the image, in pixels
  int height;            // height of the image (rows)
  int channels;          // number of color channels (1 = greyscale)

  // load the image using the BMP loader file specified in ImageUtil.h
  p_CPUImageGBR = LoadBMPImage(argv[1],&width, &height, &channels);
  if(!p_CPUImageGBR)
    {
    printf("Unable to load image\nPress Enter to Exit...\n");
    getchar();
    return(-1);
  }

  if(channels != 3)
  {
    printf("This example expects a 3-channel color Image.");
    getchar();
    return(-1);
  }

#ifdef WIN32
  // Create an window to draw the image
  SimpleCudaGLWindow wnd;
  wnd.CreateCudaGLWindow(50,50,width,height,"Image Rotate Example");
  wnd.SetVSync(true);
  wnd.Flip = true; // using .BMP, so need to flip
#endif

  
  // Create a temporary image for the GBR 3-byte per pixel image we are starting with.
  // We need to convert this temporary BGR image into a 4-byte RGBA image 
  unsigned char * p_GPUtemp;
  c_err = cudaMalloc((void**)&p_GPUtemp,width*height*3);

  // copy the CPU image to the GPU
  c_err = cudaMemcpy(p_GPUtemp,p_CPUImageGBR,width*height*3,cudaMemcpyHostToDevice);
  

  // Allocate Memory for the input and output image
  // Allocate memory on the GPU for the image.  We'll use cudaMallocPitch, to pad the width if needed
  // to ensure that column 0 is always on a 64-byte boundary
  uchar4 * p_GPUImage;          // a pointer for the location of the image in GPU memory. 
  size_t pitch;            // pitch is the number of bytes between each row
  c_err = cudaMallocPitch((void**)&p_GPUImage,&pitch,width*sizeof(uchar4),height);

  // Call a GPU function to convert the BGR input image into and RGBimage
  c_err = convertBGRtoRGBA(p_GPUtemp,p_GPUImage,width,height,pitch);

  // Use a CUDA Array to store the input image
  cudaArray * inputArray;
  // The channel format descriptor tells the hardware how to interpret the pixel data, in this case a 4-channel,4-byte 
  cudaChannelFormatDesc uchar4_desc = cudaCreateChannelDesc<uchar4>();  
  c_err = cudaMallocArray(&inputArray,&uchar4_desc,width,height);

  // Now copy the RGBA image to a cudaArray for texturing. Since the source image is now in an array, p_GPUImage can be used for destination
  c_err = cudaMemcpy2DToArray(inputArray,0,0,p_GPUImage,pitch,width*sizeof(uchar4),height,cudaMemcpyDeviceToDevice);
  
  // We are done setting up.  Now lets rotate the image in a loop
  printf("Press any key to quit...\n");


  float angle = 0;
  while(!_kbhit())
  {
    c_err = RotateImage(inputArray,p_GPUImage,width,height,pitch,angle);
#ifdef WIN32
    wnd.DrawImage((unsigned char *)p_GPUImage,width,height,pitch,GL_RGBA,GL_UNSIGNED_BYTE);
    wnd.CheckWindowsMessages(); // keep the window alive
#endif
    angle += 1.0f;
  }
  
  // Free the memory we have used
  free(p_CPUImageGBR);
  cudaFree(p_GPUtemp);
  cudaFree(p_GPUImage);
  cudaFreeArray(inputArray);
  return(0);

}
