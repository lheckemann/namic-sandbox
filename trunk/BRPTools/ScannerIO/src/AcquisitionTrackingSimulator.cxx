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

#include <math.h>

#include "igtlOSUtil.h"
#include "igtlTransformMessage.h"
#include "AcquisitionTrackingSimulator.h"

AcquisitionTrackingSimulator::AcquisitionTrackingSimulator()
{
  this->Interval_ms = 100;  // default
}

AcquisitionTrackingSimulator::~AcquisitionTrackingSimulator()
{
}

int AcquisitionTrackingSimulator::Init()
{
  return 1;
}

int AcquisitionTrackingSimulator::StartScan()
{
  this->Run();
  return 1;
}

int AcquisitionTrackingSimulator::PauseScan()
{
  this->Sleep();
  return 1;
}

int AcquisitionTrackingSimulator::StopScan()
{
  this->Stop();
  return 1;
}

int AcquisitionTrackingSimulator::SetMatrix(float* matrix)
{
  std::cerr << "AcquisitionGEExcite::SetMatrix() called." << std::endl;
  std::cerr << "matrix = " << std::endl;
  std::cerr << "    " << matrix[0] << ", " << matrix[1] << ", " << matrix[2] << std::endl;
  std::cerr << "    " << matrix[3] << ", " << matrix[4] << ", " << matrix[5] << std::endl;
  std::cerr << "    " << matrix[6] << ", " << matrix[7] << ", " << matrix[8] << std::endl;
  std::cerr << "    " << matrix[9] << ", " << matrix[10] << ", " << matrix[11] << std::endl;

  return 1;
}

void AcquisitionTrackingSimulator::Process()
{
  igtl::TransformMessage::Pointer frame = igtl::TransformMessage::New();
  igtl::Matrix4x4 matrix;
  this->SetCircularFrameBufferSize(3);

  frame->SetDeviceName("Tracker");

  while (1)
    {
      this->GetRandomTestMatrix(matrix);
      frame->SetMatrix(matrix);
      frame->Pack();
      if (this->PostProcessThread)
        {
          int id = this->PutFrameToBuffer(static_cast<igtl::MessageBase::Pointer>(frame));
          std::cerr << "AcquisitionTrackingSimulator::Process(): frame in buffer #" <<  id << std::endl;
          this->PostProcessThread->PullTrigger((void*)id);
        }
      igtl::Sleep(this->Interval_ms);
    }
}


void AcquisitionTrackingSimulator::SetFrameRate(float fps)
{
  this->Interval_ms = (int) (1000 / fps);
}

void AcquisitionTrackingSimulator::GetRandomTestMatrix(igtl::Matrix4x4& matrix)
{
  float position[3];
  float orientation[4];

  // random position
  static float phi = 0.0;
  position[0] = 50.0 * cos(phi);
  position[1] = 50.0 * sin(phi);
  position[2] = 0;
  phi = phi + 0.2;

  // random orientation
  static float theta = 0.0;
  orientation[0]=0.0;
  orientation[1]=0.6666666666*cos(theta);
  orientation[2]=0.577350269189626;
  orientation[3]=0.6666666666*sin(theta);
  theta = theta + 0.1;

  //igtl::Matrix4x4 matrix;
  igtl::QuaternionToMatrix(orientation, matrix);

  matrix[0][3] = position[0];
  matrix[1][3] = position[1];
  matrix[2][3] = position[2];
  
  igtl::PrintMatrix(matrix);
}

