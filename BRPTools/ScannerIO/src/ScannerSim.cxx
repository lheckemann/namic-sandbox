#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "igtlImage.h"
#include "ScannerSim.h"

ScannerSim::ScannerSim()
{
  numImages = 0;
  image = NULL;
}

ScannerSim::~ScannerSim()
{
}

igtl::Image* ScannerSim::GetCurrentFrame()
{
}

int ScannerSim::Init()
{
}

int ScannerSim::Start()
{
}

int ScannerSim::Pause()
{
}

int ScannerSim::Stop()
{
}

int ScannerSim::SetMatrix(float* matrix)
{
}

int ScannerSim::LoadImageData(char* fnameTemp, int bindex, int eindex,
                              int scalarType, int size[3], float spacing[3])
{
  if (numImages > 0)
    {
      DeleteImages();
    }

  char filename[128];
  int nframes = eindex - bindex + 1;

  image = new igtl::Image*[nframes];
  
  for (int i = 0; i < nframes; i ++)
    {
      // allocate new igtl::Image
      image[i] = igtl::Image::New();
      image[i]->SetDimensions(size);
      image[i]->SetSpacing(spacing);
      image[i]->SetScalarType(scalarType);
      image[i]->AllocateScalars();

      // generate file name
      sprintf(filename, fnameTemp, i+bindex);
      std::cerr << "Reading " << filename << "..." << std::endl;
      FILE *fp = fopen(filename, "rb");
      if (fp == NULL) {
        std::cerr << "File opeining error: " << filename << std::endl;
        return NULL;
      }

      // read image from raw data
      int fsize = size[0]*size[1]*size[2]*image[i]->GetScalarSize();
      size_t b = fread(image[i]->GetScalarPointer(), 1, fsize, fp);
      fclose(fp);
      if (b != fsize)
        {
          std::cerr << "File reading error: " << filename << std::endl;
          std::cerr << "   File size: " << fsize << std::endl;
          std::cerr << "   Read data: " << b << std::endl;
          return NULL;
        }
    }
  return 1;
}

int ScannerSim::DeleteImages()
{
  for (int i = 0; i < numImages; i ++)
    {
      image[i]->Delete();
    }

  delete image;
  numImages = 0;
}


