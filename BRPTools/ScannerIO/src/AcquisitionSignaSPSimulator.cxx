/*=========================================================================

  Program:   Imaging Scanner Interface
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>  // is it OK for windows?

#include <math.h>

#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "AcquisitionSignaSPSimulator.h"
#include "io_signa5.h"

AcquisitionSignaSPSimulator::AcquisitionSignaSPSimulator()
{
  this->CurrentFrameIndex = 0;
  this->CurrentFrameSliceIndex = 0;
  this->imageArray.clear();
  this->Interval_ms = 100;  // default
  this->CurrentFrame = igtl::ImageMessage::New();
}

AcquisitionSignaSPSimulator::~AcquisitionSignaSPSimulator()
{
}

int AcquisitionSignaSPSimulator::Init()
{
  return 1;
}

int AcquisitionSignaSPSimulator::StartScan()
{
  this->Run();
  return 1;
}

int AcquisitionSignaSPSimulator::PauseScan()
{
  this->Sleep();
  return 1;
}

int AcquisitionSignaSPSimulator::StopScan()
{
  this->Stop();
  return 1;
}

int AcquisitionSignaSPSimulator::SetMatrix(float* matrix)
{
  std::cerr << "AcquisitionGEExcite::SetMatrix() called." << std::endl;
  std::cerr << "matrix = " << std::endl;
  std::cerr << "    " << matrix[0] << ", " << matrix[1] << ", " << matrix[2] << std::endl;
  std::cerr << "    " << matrix[3] << ", " << matrix[4] << ", " << matrix[5] << std::endl;
  std::cerr << "    " << matrix[6] << ", " << matrix[7] << ", " << matrix[8] << std::endl;
  std::cerr << "    " << matrix[9] << ", " << matrix[10] << ", " << matrix[11] << std::endl;

  return 1;
}

void AcquisitionSignaSPSimulator::Process()
{
  igtl::ImageMessage::Pointer frame;
  this->SetCircularFrameBufferSize(3);

  while (1)
    {
      GetCurrentFrame(frame);
      if (this->PostProcessThread)
        {
          int id = this->PutFrameToBuffer(frame);
          std::cerr << "AcquisitionSignaSPSimulator::Process(): frame in bffer #" <<  id << std::endl;
          this->PostProcessThread->PullTrigger((void*)id);
        }
      igtl::Sleep(this->Interval_ms);
    }
}


void AcquisitionSignaSPSimulator::SetFrameRate(float fps)
{
  this->Interval_ms = (int) (1000 / fps);
}

int AcquisitionSignaSPSimulator::LoadImageData(char* fnameTemp, int bindex, int eindex)
{
  DeleteImages();

  char filename[128];
  int nframes = eindex - bindex + 1;

  int size[3];
  //int scalarType = 6;
  float spacing[3];
  float origin[3];

  igtl::Matrix4x4 matrix;
  float v0[3];
  float v1[3];
  float v2[3];
  float l;

  GenesisImageInfo* imageInfo;
  
  this->imageArray.clear();
  for (int i = 0; i < nframes; i ++)
    {

      // generate file name
      
      sprintf(filename, fnameTemp, i+bindex);
      std::cerr << "Reading " << filename << "..." << std::endl;

      struct stat st;
      if(stat(filename,&st) != 0)
        {
          std::cerr << "File: " << filename << " does not exist..." << std::endl;
          continue;
        }

      imageInfo = (GenesisImageInfo*)malloc(sizeof(GenesisImageInfo));
      readGenesisFile(filename, imageInfo);
      printGenesisImageInfo(imageInfo);

      size[0] = imageInfo->ih.matrixX;
      size[1] = imageInfo->ih.matrixY;
      size[2] = 1;

      spacing[0] = imageInfo->ih.pixSizeX;
      spacing[1] = imageInfo->ih.pixSizeY;
      spacing[2] = imageInfo->ih.sliceThick;

      origin[0] = imageInfo->ih.imgCR;
      origin[1] = imageInfo->ih.imgCA;
      origin[2] = imageInfo->ih.imgCS;

      v0[0] = imageInfo->ih.imgTrhcR - imageInfo->ih.imgTlhcR;
      v0[1] = imageInfo->ih.imgTrhcA - imageInfo->ih.imgTlhcA;
      v0[2] = imageInfo->ih.imgTrhcS - imageInfo->ih.imgTlhcS;
      l = sqrt(v0[0]*v0[0] + v0[1]*v0[1] + v0[2]*v0[2]);
      v0[0] /= l;
      v0[1] /= l;
      v0[2] /= l;
      matrix[0][0] = v0[0];  // RL
      matrix[1][0] = v0[1];  // AP
      matrix[2][0] = v0[2];  // SI
      
      v1[0] = imageInfo->ih.imgTrhcR - imageInfo->ih.imgBrhcR;
      v1[1] = imageInfo->ih.imgTrhcA - imageInfo->ih.imgBrhcA;
      v1[2] = imageInfo->ih.imgTrhcS - imageInfo->ih.imgBrhcS;
      l = sqrt(v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2]);
      v1[0] /= l;
      v1[1] /= l;
      v1[2] /= l;
      matrix[0][1] = v1[0];  // RL
      matrix[1][1] = v1[1];  // AP
      matrix[2][1] = v1[2];  // SI

      igtl::Cross(v2, v0, v1);
      matrix[0][2] = v2[0];  // RL
      matrix[1][2] = v2[1];  // AP
      matrix[2][2] = v2[2];  // SI
      
      matrix[0][3] = origin[0];  // RL
      matrix[1][3] = origin[1];  // AP
      matrix[2][3] = origin[2];  // SI

      igtl::PrintMatrix(matrix);

      igtl::ImageMessage::Pointer im = igtl::ImageMessage::New();
      imageArray.push_back(im);
      im->SetDimensions(size);
      im->SetSpacing(spacing);
      //im->SetScalarType(scalarType);
      im->SetScalarTypeToInt16();
      im->SetOrigin(origin);
      im->SetMatrix(matrix);
      im->SetDeviceName("Scanner");
      im->AllocateScalars();

      // read image from raw data
      memcpy(im->GetScalarPointer(), imageInfo->imageData, im->GetImageSize());
      im->Pack();

      freeGenesisImageData(imageInfo);
      free(imageInfo);
      
    }

  // Set sub-volume size / offset /step
  SetSubVolumeDimension(size);
  
  return 1;
}

int AcquisitionSignaSPSimulator::SetSubVolumeDimension(int dim[3])
{
  this->SubVolumeDimension[0] = dim[0];
  this->SubVolumeDimension[1] = dim[1];
  this->SubVolumeDimension[2] = dim[2];
  return 1;
}

void AcquisitionSignaSPSimulator::GetRandomTestMatrix(igtl::Matrix4x4& matrix)
{
  float position[3];
  float orientation[4];

  // random position
  static float phi = 0.0;
  position[0] = 50.0 * cos(phi);
  position[1] = 50.0 * sin(phi);
  position[2] = 0;
  phi = phi + 0.2;

  position[0] = 0;
  position[1] = 0;
  position[2] = 0;
  
  // random orientation
  static float theta = 0.0;
  orientation[0]=0.0;
  orientation[1]=0.6666666666*cos(theta);
  orientation[2]=0.577350269189626;
  orientation[3]=0.6666666666*sin(theta);
  theta = theta + 0.1;

  orientation[0]=0.0;
  orientation[1]=0.0;
  orientation[2]=0.0;
  orientation[3]=1.0;
  
  //igtl::Matrix4x4 matrix;
  igtl::QuaternionToMatrix(orientation, matrix);

  matrix[0][3] = position[0];
  matrix[1][3] = position[1];
  matrix[2][3] = position[2];
  
  igtl::PrintMatrix(matrix);
}

void AcquisitionSignaSPSimulator::GetCurrentFrame(igtl::ImageMessage::Pointer& cf)
{

  cf = imageArray[this->CurrentFrameIndex];
  this->CurrentFrameIndex ++;
  if (this->CurrentFrameIndex >= imageArray.size())
    {
      this->CurrentFrameIndex = 0;
    }
    
  /*
  igtl::ImageMessage::Pointer currentVolumePtr;
  int dim[3];
  float spacing[3];
  int scalarType;

  currentVolumePtr = imageArray[this->CurrentFrameIndex];
  currentVolumePtr->GetDimensions(dim);
  currentVolumePtr->GetSpacing(spacing);

  int svd[3];
  int svoff[3];
  currentVolumePtr->GetSubVolume(svd, svoff);
  scalarType = currentVolumePtr->GetScalarType();
  
  // Set properties for current frame buffer
  this->CurrentFrame->SetDimensions(dim);
  this->CurrentFrame->SetSpacing(spacing);
  this->CurrentFrame->SetScalarType(scalarType);
  this->CurrentFrame->SetDeviceName("Scanner");

  int subVolOff[3];
  subVolOff[0] = 0;
  subVolOff[1] = 0;
  subVolOff[2] = this->CurrentFrameSliceIndex * this->SubVolumeDimension[2];

  std::cerr << "GetCurrentFrame(): frame index " << this->CurrentFrameIndex <<std::endl;
  std::cerr << "GetCurrentFrame(): slice index " << this->CurrentFrameSliceIndex <<std::endl;

  std::cerr << "SubVolumeDimension = ( "
            << SubVolumeDimension[0] << ", "
            << SubVolumeDimension[1] << ", "
            << SubVolumeDimension[2] << ")" << std::endl;

  this->CurrentFrame->SetSubVolume(SubVolumeDimension, subVolOff);
  this->CurrentFrame->AllocateScalars();

  currentVolumePtr->GetSubVolume(svd, svoff);
  scalarType = currentVolumePtr->GetScalarType();
  
  char* srcPtr = (char*)currentVolumePtr->GetScalarPointer();
  char* dstPtr = (char*)this->CurrentFrame->GetScalarPointer();

  int sizei    = dim[0];
  int sizej    = dim[1];
  int sizek    = dim[2];
  int subsizei = this->SubVolumeDimension[0];
  int subsizej = this->SubVolumeDimension[1];
  int subsizek = this->SubVolumeDimension[2];
  int bg_i     = subVolOff[0];
  int ed_i     = bg_i + this->SubVolumeDimension[0];
  int bg_j     = subVolOff[1];
  int ed_j     = bg_j + this->SubVolumeDimension[1];
  int bg_k     = subVolOff[2];
  int ed_k     = bg_k + this->SubVolumeDimension[2];

  int scalarSize = this->CurrentFrame->GetScalarSize();

  for (int k = bg_k; k < ed_k; k ++)
    {
      for (int j = bg_j; j < ed_j; j ++)
        {
          memcpy(&dstPtr[(subsizei*subsizej*(k-bg_k) + subsizei*j)*scalarSize],
                 &srcPtr[(sizei*sizej*k + sizei*j + bg_i)*scalarSize],
                 subsizei*scalarSize);
        }
    }

  //// Set random matrix
  //igtl::Matrix4x4 matrix;
  //GetRandomTestMatrix(matrix);
  //this->CurrentFrame->SetMatrix(matrix);
  this->CurrentFrame->Pack();
  std::cerr << "packed" <<std::endl;
  
  this->CurrentFrameSliceIndex ++;
  if ((this->CurrentFrameSliceIndex+1)*(this->SubVolumeDimension[2]) > dim[2])
    {
      this->CurrentFrameIndex ++;
      this->CurrentFrameSliceIndex = 0;
    }
  
  if (this->CurrentFrameIndex >= imageArray.size())
    {
      this->CurrentFrameIndex = 0;
    }

  cf = this->CurrentFrame;
  */
}


int AcquisitionSignaSPSimulator::DeleteImages()
{
  std::vector<igtl::ImageMessage::Pointer>::iterator iter;
  for (iter = imageArray.begin(); iter != imageArray.end(); iter ++)
    {
      (*iter)->Delete();
    }
  imageArray.clear();
  CurrentFrameIndex = 0;
  CurrentFrameSliceIndex = 0;
  /*
  if (CurrentFrame)
    {
      this->CurrentFrame->Delete();
      this->CurrentFrame = NULL;
    }
  */
  return 1;
}



