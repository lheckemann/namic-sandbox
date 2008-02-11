#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "igtlImageMessage.h"
#include "ScannerSim.h"

ScannerSim::ScannerSim()
{
  currentFrameIndex = 0;
  currentFrameSliceIndex = 0;
  imageArray.clear();
}

ScannerSim::~ScannerSim()
{
}

igtl::ImageMessage::Pointer ScannerSim::GetCurrentFrame()
{
  igtl::ImageMessage::Pointer currentVolumePtr;

  currentVolumePtr = imageArray[currentFrameIndex];

  int dim[3];
  int subVolDim[3];
  int subVolOff[3];
  currentVolumePtr->GetDimensions(dim);
  currentFrame->GetSubVolume(subVolDim, subVolOff);
  char* srcPtr = (char*)currentVolumePtr->GetScalarPointer();
  
  // Shift the sub-volume
  subVolOff[2] = subVolDim[2] * currentFrameSliceIndex;
  currentFrame->SetSubVolume(subVolDim, subVolOff);
  // Memory allocation is not required here, since the size doesn't change.
  //   currentFrame->AllocateScalars();
  char* dstPtr = (char*)currentFrame->GetScalarPointer();

  int sizei = dim[0];
  int sizej = dim[1];
  int sizek = dim[2];
  int subsizei = subVolDim[0];
  int subsizej = subVolDim[1];
  int subsizek = subVolDim[2];
  int bg_i  = subVolOff[0];
  int ed_i  = bg_i + subVolDim[0];
  int bg_j  = subVolOff[1];
  int ed_j  = bg_j + subVolDim[1];
  int bg_k  = subVolOff[2];
  int ed_k  = bg_k + subVolDim[2];
  int scalarSize = currentFrame->GetScalarSize();

  std::cerr << "Offset = ("
            << subVolOff[0] << ", "
            << subVolOff[1] << ", "
            << subVolOff[2] << ") "
            << std::endl;

  for (int k = bg_k; k < ed_k; k ++)
    {
      for (int j = bg_j; j < ed_j; j ++)
        {
          memcpy(&dstPtr[(subsizei*subsizej*(k-bg_k) + subsizei*j)*scalarSize],
                 &srcPtr[(sizei*sizej*k + sizei*j + bg_i)*scalarSize],
                 subsizei*scalarSize);
        }
    }
  
  currentFrameSliceIndex ++;
  if (currentFrameSliceIndex*subVolDim[2] >= dim[2])
    {
      currentFrameIndex ++;
      currentFrameSliceIndex = 0;
    }

  if (currentFrameIndex >= imageArray.size())
    {
      currentFrameIndex = 0;
    }
  return currentFrame;
}

int ScannerSim::Init()
{
  currentFrameIndex = 0;
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
      igtl::ImageMessage::Pointer im = igtl::ImageMessage::New();
      imageArray.push_back(im);
      im->SetDimensions(size);
      im->SetSpacing(spacing);
      im->SetScalarType(scalarType);
      im->SetDeviceName("Scanner");
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
      
      // Allocate current frame buffer
      currentFrame = igtl::ImageMessage::New();
      currentFrame->SetDimensions(size);
      currentFrame->SetSpacing(spacing);
      currentFrame->SetScalarType(scalarType);
      currentFrame->SetSubVolume(size[0], size[1], 1, 0, 0, 0);
      currentFrame->SetDeviceName("Scanner");
      currentFrame->AllocateScalars();

    }
  return 1;
}

int ScannerSim::DeleteImages()
{
  std::vector<igtl::ImageMessage::Pointer>::iterator iter;
  for (iter = imageArray.begin(); iter != imageArray.end(); iter ++)
    {
      (*iter)->Delete();
    }
  imageArray.clear();
  currentFrameIndex = 0;
  currentFrameSliceIndex = 0;
  if (currentFrame)
    currentFrame->Delete();
}

