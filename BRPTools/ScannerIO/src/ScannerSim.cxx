#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "igtlImage.h"
#include "ScannerSim.h"

ScannerSim::ScannerSim()
{
  currentFrame = 0;
  imageArray.clear();
}

ScannerSim::~ScannerSim()
{
}

igtl::Image::Pointer ScannerSim::GetCurrentFrame()
{
  igtl::Image::Pointer ptr;

  ptr = imageArray[currentFrame];
  currentFrame ++;
  if (currentFrame >= imageArray.size())
    {
      currentFrame = 0;
    }
  return ptr;
}

int ScannerSim::Init()
{
  currentFrame = 0;
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
  DeleteImages();

  char filename[128];
  int nframes = eindex - bindex + 1;

  for (int i = 0; i < nframes; i ++)
    {
      igtl::Image::Pointer im = igtl::Image::New();
      imageArray.push_back(im);
      im->SetDimensions(size);
      im->SetSpacing(spacing);
      im->SetScalarType(scalarType);
      im->AllocateScalars();

      // generate file name
      sprintf(filename, fnameTemp, i+bindex);
      std::cerr << "Reading " << filename << "..." << std::endl;
      FILE *fp = fopen(filename, "rb");
      if (fp == NULL) {
        std::cerr << "File opeining error: " << filename << std::endl;
        return NULL;
      }

      // read image from raw data
      int fsize = im->GetImageSize();
      size_t b = fread(im->GetScalarPointer(), 1, fsize, fp);
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
  std::vector<igtl::Image::Pointer>::iterator iter;
  for (iter = imageArray.begin(); iter != imageArray.end(); iter ++)
    {
      (*iter)->Delete();
    }
  imageArray.clear();
  currentFrame = 0;
}

