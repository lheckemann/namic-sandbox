/*=========================================================================

  Program:   Open IGT Link -- Performance Test Prgram
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

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlImageMessage.h"
#include "igtlStatusMessage.h"
#include "igtlServerSocket.h"

#include "igtlLogger.h"
#include "igtlTimeMeasure.h"

int ReceiveTransform(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header);
int ReceiveImage(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header);
int ReceiveStatus(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header, igtl::Logger::Pointer& logger, igtl::TimeMeasure::Pointer& time);

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 2) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <port>"    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    exit(0);
    }

  int    port     = atoi(argv[1]);

  igtl::ServerSocket::Pointer serverSocket;
  serverSocket = igtl::ServerSocket::New();
  serverSocket->CreateServer(port);

  // socket for receiving data
  igtl::Socket::Pointer socket;

  // time stamps
  igtl::TimeStamp::Pointer dataTimeStamp = igtl::TimeStamp::New();
  igtl::TimeStamp::Pointer currentTimeStamp = igtl::TimeStamp::New();
  igtl::TimeStamp::Pointer baseTimeStamp    = igtl::TimeStamp::New();

  
  while (1)
    {
    //------------------------------------------------------------
    // Waiting for Connection
    socket = serverSocket->WaitForConnection(1000);
    
    if (socket.IsNotNull()) // if client connected
      {
      // Create a message buffer to receive header
      igtl::MessageHeader::Pointer headerMsg;
      headerMsg = igtl::MessageHeader::New();

      // Prepare Logging class
      igtl::Logger::Pointer logger;
      logger = igtl::Logger::New();

      igtl::TimeMeasure::Pointer time = igtl::TimeMeasure::New();

      baseTimeStamp->GetTime();

      //------------------------------------------------------------
      // loop
      while(1)
        {

        // Initialize receive buffer
        headerMsg->InitPack();

        // Receive generic header from the socket
        int r = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
        if (r != headerMsg->GetPackSize())
          {
          std::cerr << "Error: receiving data." << std::endl;
          break;
          }

        // Deserialize the header
        headerMsg->Unpack();
        headerMsg->GetTimeStamp(dataTimeStamp);

        // Check data type and receive data body
        if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0)
          {
          ReceiveTransform(socket, headerMsg);
          // Retrive the timestamp
          currentTimeStamp->GetTime();
          double delay = currentTimeStamp->GetTimeStamp() - dataTimeStamp->GetTimeStamp();
          double data[2];
          data[0] = currentTimeStamp->GetTimeStamp() - baseTimeStamp->GetTimeStamp();
          data[1] = delay;
          logger->PushData(data);
          }
        else if (strcmp(headerMsg->GetDeviceType(), "IMAGE") == 0)
          {
          ReceiveImage(socket, headerMsg);
          // Retrive the timestamp
          currentTimeStamp->GetTime();
          double delay = currentTimeStamp->GetTimeStamp() - dataTimeStamp->GetTimeStamp();
          double data[2];
          data[0] = currentTimeStamp->GetTimeStamp() - baseTimeStamp->GetTimeStamp();
          data[1] = delay;
          logger->PushData(data);
          }
        else if (strcmp(headerMsg->GetDeviceType(), "STATUS") == 0)
          {
          ReceiveStatus(socket, headerMsg, logger, time);
          }
        else
          {
          // if the data type is unknown, skip reading.
          socket->Skip(headerMsg->GetBodySizeToRead());
          }
        }
      socket->CloseSocket();
      }
    }
    
  //------------------------------------------------------------
  // Close connection (The example code never reachs to this section ...)
  
  socket->CloseSocket();

}


int ReceiveTransform(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{

  //std::cerr << "Receiving TRANSFORM data type." << std::endl;
  
  // Create a message buffer to receive transform data
  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetMessageHeader(header);
  transMsg->AllocatePack();
  
  // Receive transform data from the socket
  socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = transMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    // Retrive the transform data
    igtl::Matrix4x4 matrix;
    transMsg->GetMatrix(matrix);
    //igtl::PrintMatrix(matrix);
    return 1;
    }

  return 0;

}


int ReceiveImage(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{
  //std::cerr << "Receiving IMAGE data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::ImageMessage::Pointer imgMsg;
  imgMsg = igtl::ImageMessage::New();
  imgMsg->SetMessageHeader(header);
  imgMsg->AllocatePack();
  
  // Receive transform data from the socket
  socket->Receive(imgMsg->GetPackBodyPointer(), imgMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = imgMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    // Retrive the image data
    int   size[3];          // image dimension
    float spacing[3];       // spacing (mm/pixel)
    int   svsize[3];        // sub-volume size
    int   svoffset[3];      // sub-volume offset
    int   scalarType;       // scalar type

    scalarType = imgMsg->GetScalarType();
    imgMsg->GetDimensions(size);
    imgMsg->GetSpacing(spacing);
    imgMsg->GetSubVolume(svsize, svoffset);

    /*
    std::cerr << "Device Name           : " << imgMsg->GetDeviceName() << std::endl;
    std::cerr << "Scalar Type           : " << scalarType << std::endl;
    std::cerr << "Dimensions            : ("
              << size[0] << ", " << size[1] << ", " << size[2] << ")" << std::endl;
    std::cerr << "Spacing               : ("
              << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << ")" << std::endl;
    std::cerr << "Sub-Volume dimensions : ("
              << svsize[0] << ", " << svsize[1] << ", " << svsize[2] << ")" << std::endl;
    std::cerr << "Sub-Volume offset     : ("
              << svoffset[0] << ", " << svoffset[1] << ", " << svoffset[2] << ")" << std::endl;
    */

    return 1;
    }

  return 0;

}


int ReceiveStatus(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header, 
                  igtl::Logger::Pointer& logger, igtl::TimeMeasure::Pointer& time)
{

  //std::cerr << "Receiving STATUS data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::StatusMessage::Pointer statusMsg;
  statusMsg = igtl::StatusMessage::New();
  statusMsg->SetMessageHeader(header);
  statusMsg->AllocatePack();
  
  // Receive transform data from the socket
  socket->Receive(statusMsg->GetPackBodyPointer(), statusMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = statusMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    std::cerr << "========== STATUS ==========" << std::endl;
    std::cerr << " Code      : " << statusMsg->GetCode() << std::endl;
    std::cerr << " SubCode   : " << statusMsg->GetSubCode() << std::endl;
    std::cerr << " Error Name: " << statusMsg->GetErrorName() << std::endl;
    std::cerr << " Status    : " << statusMsg->GetStatusString() << std::endl;
    std::cerr << "============================" << std::endl;
    }

  if ((int)statusMsg->GetSubCode() > 0) // new log session
    {
    // Set Logger
    logger->SetFileName(statusMsg->GetStatusString());
    logger->SetSize((int)statusMsg->GetSubCode(), 2);
    std::cerr << "The logger is ready." << std::endl;
    time->SetName(statusMsg->GetStatusString());
    time->Start();
    }
  else if (strcmp(logger->GetFileName(), statusMsg->GetStatusString()) == 0)
    {
    time->End();
    time->PrintResult();
    std::cerr << "Writing Log file: " << logger->GetFileName() << std::endl;
    logger->Flush();
    }

  return 0;

}
