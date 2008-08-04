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
#include <unistd.h>

#include "igtlOSUtil.h"
#include "igtlTransformMessage.h"
#include "igtlImageMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"

#include "igtlLoopController.h"
#include "igtlTimeMeasure.h"

unsigned short* GenerateDummyImage(int x, int y, int z);
void GetRandomTestMatrix(igtl::Matrix4x4& matrix);
int GetDummyImage(igtl::ImageMessage::Pointer& msg, unsigned short *dummy, int index);

enum {
  TYPE_TRANSFORM,
  TYPE_IMAGE
};

void PrintUsage(const char *program)
{

  std::cerr << "Usage: " << program << " [OPTIONS]" << std::endl;
  std::cerr << std::endl;
  std::cerr << "    -h HOSTNAME    IP or host name" << std::endl;
  std::cerr << "    -p PORT        Port # (18944 in Slicer default" << std::endl;
  std::cerr << "    -t FILENAME    Test name string, which will be used as a filename in the server." << std::endl;
  std::cerr << "    -d NUMBER      Duration in seconds" << std::endl;  
  std::cerr << "    -f NUMBER      Frequency (fps) to send coordinate" << std::endl;
  std::cerr << "    -c NUMBER      Number of channels for coordinates" << std::endl;
  std::cerr << "    -F NUMBER      Frequency (fps) to send image" << std::endl;
  std::cerr << "    -C NUMBER      Number of channels for image" << std::endl;
  std::cerr << "    -x NUMBER      Number of pixels in x direction" << std::endl;
  std::cerr << "    -y NUMBER      Number of pixels in y direction" << std::endl;
  std::cerr << "    -z NUMBER      Number of pixels in z direction" << std::endl;
  std::cerr << std::endl;
}


int main(int argc, char* argv[])
{

  std::string hostname;
  std::string testName;
  int    port     = -1;
  double duration = 0.0;
  double trackFps = 0.0;
  int    trackChannels = 1;
  double imageFps = 0.0;
  int    imageChannels = 1;
  int    x = 0;
  int    y = 0;
  int    z = 0;

  //------------------------------------------------------------
  // Parse Arguments

  int c;
  while ((c = getopt(argc, argv, "?h:p:t:d:f:c:F:C:x:y:z:")) != -1)
    {
    switch(c)
      {
      case '?':   // proint help
        PrintUsage(argv[0]);
        exit(0);
        break;
      case 'h':   // hostname or IIP address
        hostname = optarg;
        break;
      case 'p':   // port number
        port = atoi(optarg);
        break;
      case 't':   // test data name
        testName = optarg;
        break;
      case 'd':   // duration
        duration = atof(optarg);
        break;
      case 'f':   // frame rate for tracking
        trackFps = atof(optarg);
        break;
      case 'c':   // number of channels for tracking
        trackChannels = atoi(optarg);
        break;
      case 'F':   // frame rate for imaging
        imageFps = atof(optarg);
        break;
      case 'C':   // number of channels for imaging
        imageChannels = atoi(optarg);
        break;
      case 'x':   // image resolution -- x
        x = atoi(optarg);
        break;
      case 'y':   // image resolution -- y
        y = atoi(optarg);
        break;
      case 'z':   // image resolution -- z
        z = atoi(optarg);
        break;
      case ':':
        PrintUsage(argv[0]);
        exit(0);
        break;
      }
    }

  if (hostname.size() == 0 || 
      testName.size() == 0 ||
      port <= 0            ||
      duration <= 0.0      ||
      trackChannels < 1    ||
      imageChannels < 1)
    {
    PrintUsage(argv[0]);
    exit(0);
    }


  //------------------------------------------------------------
  // Generate Time Table

  igtl::LoopController::TimeTableType timeTable;

  int nTrackData = 0;
  int nImageData = 0;
  double tTrack  = duration + 1.0;
  double tImage  = duration + 1.0; 
  double trackInterval = 0.0;
  double imageInterval = 0.0;

  if (trackFps > 0)
    {
    nTrackData = static_cast<int>(floor(duration * trackFps) + 1);
    tTrack = 0.0;
    trackInterval = 1.0 / trackFps;
    }
  if (imageFps > 0)
    {
    nImageData = static_cast<int>(floor(duration * imageFps) + 1);
    tImage = 0.0;
    imageInterval = 1.0 / imageFps;
    }

  int nData = nTrackData + nImageData;
  //TimeTableElem* timeTable = new TimeTableElem[nData];

  int i = 0;
  double t = 0.0;

  while (i < nData)
    {
    if (tTrack <= tImage)
      {
      igtl::LoopController::TimeTableElement tme;
      tme.time = tTrack;
      tme.type = TYPE_TRANSFORM;
      timeTable.push_back(tme);
      tTrack += trackInterval;
      }
    else
      {
      igtl::LoopController::TimeTableElement tme;
      tme.time = tImage;
      tme.type = TYPE_IMAGE;
      timeTable.push_back(tme);
      tImage += imageInterval;
      }

    //std::cerr << "timeTable[" << i << "] = {" << timeTable[i].type << ", " << timeTable[i].time << "}" << std::endl;
    i ++;

    }

  //------------------------------------------------------------
  // Establish Connection

  igtl::ClientSocket::Pointer socket;
  socket = igtl::ClientSocket::New();
  int r = socket->ConnectToServer(hostname.c_str(), port);

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
  statusMsg->SetSubCode(nData);
  statusMsg->SetErrorName("OK");
  statusMsg->SetStatusString(testName.c_str());
  statusMsg->Pack();
  socket->Send(statusMsg->GetPackPointer(), statusMsg->GetPackSize());
  igtl::Sleep(400); // wait for 400 ms
  std::cerr << "done." << std::endl;

  //------------------------------------------------------------
  // Allocate Transform Message Class

  igtl::Matrix4x4 matrix;
  igtl::IdentityMatrix(matrix);

  igtl::TransformMessage::Pointer transMsg;

  transMsg = igtl::TransformMessage::New();
  //transMsg->SetDeviceName("TestDevice");

  char** trackDeviceName;
  trackDeviceName = new char*[trackChannels];
  for (int c = 0; c < trackChannels; c ++)
    {
    trackDeviceName[c] = new char[20];
    sprintf(trackDeviceName[c], "Tracker%03d", c);
    }

  //------------------------------------------------------------
  // Allocate Image Message Class

  igtl::ImageMessage::Pointer imgMsg;
  unsigned short *dummyImage = NULL;

  // size parameters for image
  int   size[]     = {x, y, z};           // image dimension
  float spacing[]  = {1.0, 1.0, 1.0};     // spacing (mm/pixel)
  int   svsize[]   = {x, y, z};           // sub-volume size
  int   svoffset[] = {0, 0, 0};           // sub-volume offset
  int   scalarType = igtl::ImageMessage::TYPE_UINT16;// scalar type
  
  // Create a new IMAGE type message
  imgMsg = igtl::ImageMessage::New();
  imgMsg->SetDimensions(size);
  imgMsg->SetSpacing(spacing);
  imgMsg->SetScalarType(scalarType);
  //imgMsg->SetDeviceName("TestImager");
  imgMsg->SetSubVolume(svsize, svoffset);
  imgMsg->SetMatrix(matrix);
  
  imgMsg->AllocateScalars();
  
  // Generate Dummy image
  dummyImage = GenerateDummyImage(x, y, z);

  char** imageDeviceName;

  imageDeviceName = new char*[imageChannels];
  for (int c = 0; c < imageChannels; c ++)
    {
    imageDeviceName[c] = new char[20];
    sprintf(imageDeviceName[c], "Imager%03d", c);
    }


  //------------------------------------------------------------
  // Prepare Timestamp
  igtl::TimeStamp::Pointer timeStamp = igtl::TimeStamp::New();


  //------------------------------------------------------------
  // loop
  igtl::LoopController::Pointer loop = igtl::LoopController::New();

  igtl::TimeMeasure::Pointer tm = igtl::TimeMeasure::New();

  GetRandomTestMatrix(matrix);

  timeStamp->GetTime();
  tm->Start();

  for (loop->InitLoop(timeTable); loop->StartLoop(); )
    {
    int i = loop->GetCount();
    //timeStamp->SetTime(loop->GetTargetTime());
    if (!loop->IsSimultaneous()) // if this turn is not scheduled at the same time as the previous one
      {
      timeStamp->GetTime();
      }

    if (timeTable[i].type == TYPE_TRANSFORM)
      {
      for (int c = 0; c < trackChannels; c ++)
        {
        transMsg->SetDeviceName(trackDeviceName[c]);
        transMsg->SetMatrix(matrix);
        transMsg->SetTimeStamp(timeStamp);
        transMsg->Pack();
        socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());
        }
      }
    else if (timeTable[i].type == TYPE_IMAGE)
      {
      for (int c = 0; c < trackChannels; c ++)
        {
        imgMsg->SetDeviceName(imageDeviceName[c]);
        GetDummyImage(imgMsg, dummyImage, i % 2);
        imgMsg->SetTimeStamp(timeStamp);
        //GetRandomTestMatrix(matrix);
        //imgMsg->SetMatrix(matrix);
        imgMsg->Pack();
        socket->Send(imgMsg->GetPackPointer(), imgMsg->GetPackSize());
        }
      }
    
    GetRandomTestMatrix(matrix);
    }

  // acquire ending time
  tm->End();
  /*
  times(&tms_end);
  timeStamp->GetTime();
  actualTime  = timeStamp->GetTimeStamp() - actualTime;
  systemTime  = ((double)(tms_end.tms_stime - tms_start.tms_stime)) / (double)sysconf(_SC_CLK_TCK);
  userTime    = ((double)(tms_end.tms_utime - tms_start.tms_utime)) / (double)sysconf(_SC_CLK_TCK);
  */

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
  delete [] dummyImage;

  socket->CloseSocket();


  //------------------------------------------------------------
  // Output log
  fprintf(stderr, "Outputing time data...\n");
  //printf("%s, %f, %f, %f\n", testName, actualTime, userTime, systemTime);
  tm->SetName(testName.c_str());
  tm->PrintResult();

  /*
  igtl::Logger::Pointer logger;
  logger = igtl::Logger::New();
  logger->SetFileName("ClientLog.csv");
  logger->SetSize(1, 3);
  logger->SetAppendMode(1);
  double data[] =  {actualTime, userTime, systemTime}

  logger->PushData();
  logger->Flash();
  */

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
