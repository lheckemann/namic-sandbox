
#include <stdio.h>
#include "ImageUtils.h"

#ifdef WIN32
#include <conio.h>
#include "SimpleCudaGLWindow.h"
#endif


extern "C" cudaError_t Convolution3x3(unsigned char * src, unsigned char * dest, int width, int height, size_t pitch);

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
  int width;            // width of the image, in pixels
  int height;            // height of the image (rows)
  int channels;          // number of color channels (1 = greyscale)

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
    printf("This example expects a 1-channel grayscale Image.");
    getchar();
    return(-1);
  }

#ifdef WIN32
  // Create an window to draw the image
  SimpleCudaGLWindow wnd;
  wnd.CreateCudaGLWindow(50,50,width,height,"Image Convolution Example");
  wnd.SetVSync(true);
  wnd.Flip = true; // using .BMP, so need to flip
#endif

  // Allocate Memory on the GPU for both CPU & GPU images
  unsigned char * p_srcImage;
  unsigned char * p_outputImage;
  size_t image_pitch;
  cudaMallocPitch((void**)&p_srcImage,&image_pitch,width,height);
  cudaMallocPitch((void**)&p_outputImage,&image_pitch,width,height);

  // copy the source image from the CPU to the GPU
  cudaMemcpy2D(p_srcImage,image_pitch,p_CPUImage,width,width,height,cudaMemcpyHostToDevice);
  
  // Draw the image before the convolution
  wnd.DrawImage(p_srcImage,width,height,(int)image_pitch,GL_LUMINANCE,GL_UNSIGNED_BYTE);
  
#ifdef WIN32
  printf("Press Enter to Perform Convolution...\n");
  while(!_kbhit())
    {
    wnd.CheckWindowsMessages(); // keep the window alive
    }
  _getch();
#endif

  // We are done setting up.  Now it's time to call our convolution function
  Convolution3x3(p_srcImage,p_outputImage,width,height,image_pitch);
    

#ifdef WIN32
  // Finally draw our output image

  wnd.DrawImage(p_outputImage,width,height,(int)image_pitch,GL_LUMINANCE,GL_UNSIGNED_BYTE);
  
  printf("Press any key to quit\n");
  while(!_kbhit())
    {
    wnd.CheckWindowsMessages();
    }
#endif

  // Free memory
  cudaFree(p_outputImage);
  cudaFree(p_srcImage);
  free(p_CPUImage);
  
  return(0);

}
