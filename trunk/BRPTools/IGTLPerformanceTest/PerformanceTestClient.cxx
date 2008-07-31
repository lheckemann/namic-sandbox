/*=========================================================================

  Program:   Open IGT Link -- Example for Tracker Client Program
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <math.h>
#include <string.h>

#include "igtlOSUtil.h"
#include "igtlTransformMessage.h"
#include "igtlImageMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"

#include "igtlLoopController.h"


unsigned short* GenerateDummyImage(int x, int y, int z);
void GetRandomTestMatrix(igtl::Matrix4x4& matrix);
int GetDummyImage(igtl::ImageMessage::Pointer& msg, unsigned short *dummy, int index);

enum {
  TYPE_TRANSFORM,
  TYPE_IMAGE
};

void PrintUsage(const char *program)
{
  std::cerr << "Usage: " << program << " <hostname> <port> <fps> <test name> <ndata> <type> [<x> <y> <z>]"    << std::endl;
  std::cerr << "    <hostname> : IP or host name"                    << std::endl;
  std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
  std::cerr << "    <fps>      : Frequency (fps) to send coordinate" << std::endl;
  std::cerr << "    <test name>: Test name string, which will be used as a filename in the server." << std::endl;
  std::cerr << "    <ndata>    : Number of data to be sent."         << std::endl;
  std::cerr << "    <type>     : Data type (TRANSFORM or IMAGE)"     << std::endl;
}

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 7 && argc != 10) // check number of arguments
    {
    // If not correct, print usage
    PrintUsage(argv[0]);
    exit(0);
    }

  char*  hostname = argv[1];
  int    port     = atoi(argv[2]);
  double fps      = atof(argv[3]);
  //int    interval = (int) (1000.0 / fps);
  double interval = 1.0 / fps;
  char*  testName = argv[4];
  int    ndata    = atoi(argv[5]);

  int    type = -1;
  int    x = 0;
  int    y = 0;
  int    z = 0;
  if (strcmp(argv[6], "TRANSFORM") == 0)
    {
    type = TYPE_TRANSFORM;
    }
  else if (strcmp(argv[6], "IMAGE") == 0)
    {
    type = TYPE_IMAGE;
    }
  else
    {
    std::cerr << "Illegal type name." << std::endl; 
    type = -1;
    }

  if (type == TYPE_IMAGE)
    {
    if (argc != 10)
      {
      type = -1;
      }
    x = atoi(argv[7]);
    y = atoi(argv[8]);
    z = atoi(argv[9]);
    }

  if (type < 0)
    {
    PrintUsage(argv[0]);
    exit(0);
    }

  //------------------------------------------------------------
  // Establish Connection

  igtl::ClientSocket::Pointer socket;
  socket = igtl::ClientSocket::New();
  int r = socket->ConnectToServer(hostname, port);

  if (r != 0)
    {
    std::cerr << "Cannot connect to the server." << std::endl;
    exit(0);
    }

  //------------------------------------------------------------
  // Send Status, test name and number of data
  // The test name and number of data are implemented as
  // status message and sub-code.

  std::cerr << "Sending Status information...";
  igtl::StatusMessage::Pointer statusMsg;
  statusMsg = igtl::StatusMessage::New();
  statusMsg->SetDeviceName("TestDevice");

  statusMsg->SetCode(igtl::StatusMessage::STATUS_OK);
  statusMsg->SetSubCode(ndata);
  statusMsg->SetErrorName("OK");
  statusMsg->SetStatusString(testName);
  statusMsg->Pack();
  socket->Send(statusMsg->GetPackPointer(), statusMsg->GetPackSize());
  igtl::Sleep(400); // wait for 400 ms
  std::cerr << "done." << std::endl;

  //------------------------------------------------------------
  // Allocate Message Class

  igtl::TransformMessage::Pointer transMsg;
  igtl::ImageMessage::Pointer imgMsg;
  unsigned short *dummyImage = NULL;

  if (type == TYPE_TRANSFORM)
    {
    transMsg = igtl::TransformMessage::New();
    transMsg->SetDeviceName("TestDevice");
    }
  else //if (type == TYPE_IMAGE)
    {
    //------------------------------------------------------------
    // size parameters
    int   size[]     = {x, y, z};           // image dimension
    float spacing[]  = {1.0, 1.0, 1.0};     // spacing (mm/pixel)
    int   svsize[]   = {x, y, z};           // sub-volume size
    int   svoffset[] = {0, 0, 0};           // sub-volume offset
    int   scalarType = igtl::ImageMessage::TYPE_UINT16;// scalar type

    //------------------------------------------------------------
    // Create a new IMAGE type message
    imgMsg = igtl::ImageMessage::New();
    imgMsg->SetDimensions(size);
    imgMsg->SetSpacing(spacing);
    imgMsg->SetScalarType(scalarType);
    imgMsg->SetDeviceName("TestImager");
    imgMsg->SetSubVolume(svsize, svoffset);
    igtl::Matrix4x4 matrix;
    //GetRandomTestMatrix(matrix);
    igtl::IdentityMatrix(matrix);
    imgMsg->SetMatrix(matrix);

    imgMsg->AllocateScalars();

    //------------------------------------------------------------
    // Generate Dummy image
    dummyImage = GenerateDummyImage(x, y, z);

    }

  //------------------------------------------------------------
  // Prepare Timestamp
  igtl::TimeStamp::Pointer timeStamp = igtl::TimeStamp::New();

  //------------------------------------------------------------
  // loop

  igtl::LoopController::Pointer loop = igtl::LoopController::New();
  loop->InitLoop(interval);
  
  for (int i = 0; i < ndata; i ++)
    {
    loop->StartLoop();

    if (type == TYPE_TRANSFORM)
      {
      igtl::Matrix4x4 matrix;
      GetRandomTestMatrix(matrix);
      timeStamp->GetTime();
      //std::cerr << "time stamp = " << timeStamp->GetTimeStamp() << std::endl;
      transMsg->SetMatrix(matrix);
      transMsg->SetTimeStamp(timeStamp);
      transMsg->Pack();
      socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());

      }
    else if (type == TYPE_IMAGE)
      {
      timeStamp->GetTime();
      GetDummyImage(imgMsg, dummyImage, i % 2);
      imgMsg->SetTimeStamp(timeStamp);
      //GetRandomTestMatrix(matrix);
      //imgMsg->SetMatrix(matrix);
      imgMsg->Pack();
      socket->Send(imgMsg->GetPackPointer(), imgMsg->GetPackSize());
      }

    }


  //------------------------------------------------------------
  // Send Status to request the server to write the data to the file.

  std::cerr << "Request the server to output the data..." << std::endl;
  statusMsg->SetCode(igtl::StatusMessage::STATUS_OK);
  statusMsg->SetSubCode(0);
  statusMsg->SetErrorName("OK");
  statusMsg->Pack();
  socket->Send(statusMsg->GetPackPointer(), statusMsg->GetPackSize());

  //------------------------------------------------------------
  // Close connection

  if (type == TYPE_IMAGE && dummyImage != NULL)
    {
    delete [] dummyImage;
    }

  socket->CloseSocket();

}


//------------------------------------------------------------
// Function to generate random matrix.

void GetRandomTestMatrix(igtl::Matrix4x4& matrix)
{
  float position[3];
  float orientation[4];

  // random position
  static float phi = 0.0;
  position[0] = 50.0 * cos(phi);
  position[1] = 50.0 * sin(phi);
  position[2] = 50.0 * cos(phi);
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
  
  //igtl::PrintMatrix(matrix);
}


//------------------------------------------------------------
// Function to generate two dummy images (chess board pattern).

#define BLOCK_SIZE 16

unsigned short* GenerateDummyImage(int x, int y, int z)
{
  unsigned short* ptr;

  ptr = new unsigned short[x*y*z*sizeof(unsigned short)*2];

  for (int n = 0; n < 2; n ++)
    {
    for (int k = 0; k < z; k ++)
      {
      int rz = k / BLOCK_SIZE;
      for (int j = 0; j < y; j ++)
        {
        int ry = j / BLOCK_SIZE;
        for (int i = 0; i < x; i ++)
          {
          int rx = i / BLOCK_SIZE;
          int idx = rz+ry+rx+n;
          ptr[n*z*y*x + k*y*x + j*x + i] = (idx%2)? 16*idx:0;
          }
        }
      }
    }

  return ptr;

}


//------------------------------------------------------------
// Function to read test image data
int GetDummyImage(igtl::ImageMessage::Pointer& msg, unsigned short *dummy, int index)
{

  //------------------------------------------------------------
  // Load raw data from the file

  int offset = index * (msg->GetSubVolumeImageSize() / msg->GetScalarSize());
  memcpy(msg->GetScalarPointer(), &dummy[offset], msg->GetSubVolumeImageSize());


  return 1;
}
