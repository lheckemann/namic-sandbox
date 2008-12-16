/*=========================================================================

  Program:   Open IGT Link -- Example for Data Receiving Server Program
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
#include "igtlServerSocket.h"
#include "igtlStatusMessage.h"
#include "igtlVFixtureMessage.h"
#include "igtlScalarValueMessage.h"

int ReceiveTransform(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header);
int ReceiveImage(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header);
int ReceiveStatus(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header);
int ReceiveVFixture(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header);
int ReceiveScalarValue(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header);

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

  igtl::Socket::Pointer socket;
  
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

      //------------------------------------------------------------
      // loop
      for (int i = 0; i < 100; i ++)
        {

        // Initialize receive buffer
        headerMsg->InitPack();

        // Receive generic header from the socket
        int r = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
        if (r != headerMsg->GetPackSize())
          {
          std::cerr << "Error: receiving data." << std::endl;
          }

        // Deserialize the header
        headerMsg->Unpack();

        // Check data type and receive data body
        if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0)
          {
          ReceiveTransform(socket, headerMsg);
          }
        else if (strcmp(headerMsg->GetDeviceType(), "IMAGE") == 0)
          {
          ReceiveImage(socket, headerMsg);
          }
        else if (strcmp(headerMsg->GetDeviceType(), "STATUS") == 0)
          {
          ReceiveStatus(socket, headerMsg);
          }
        else if (strcmp(headerMsg->GetDeviceType(), "*VFIXTURE") == 0)
          {
          ReceiveVFixture(socket, headerMsg);
          }
                else if (strcmp(headerMsg->GetDeviceType(), "SCALARVALUE") == 0)
                    {
                    ReceiveScalarValue(socket, headerMsg);
                    }
        else
          {
          // if the data type is unknown, skip reading.
          socket->Skip(headerMsg->GetBodySizeToRead(), 0);
          }
        }
      }
    }
    
  //------------------------------------------------------------
  // Close connection (The example code never reachs to this section ...)
  
  socket->CloseSocket();

}


int ReceiveTransform(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving TRANSFORM data type." << std::endl;
  
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
    igtl::PrintMatrix(matrix);
    return 1;
    }

  return 0;

}


int ReceiveImage(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving IMAGE data type." << std::endl;

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
    return 1;
    }

  return 0;

}


int ReceiveStatus(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{

  std::cerr << "Receiving STATUS data type." << std::endl;

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

  return 0;

}


int ReceiveVFixture(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving Virtual Fixture data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::VFixtureMessage::Pointer vfMsg;
  vfMsg = igtl::VFixtureMessage::New();
  vfMsg->SetMessageHeader(header);
  vfMsg->AllocatePack();
  
  // Receive virtual fixture data from the socket
  socket->Receive(vfMsg->GetPackBodyPointer(), vfMsg->GetPackBodySize());
  
  // Deserialize the virtual fixture data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = vfMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    std::cerr << "Device Name           : " << vfMsg->GetDeviceName() << std::endl;
    std::cerr << "Number of Spheres     : " << vfMsg->GetNumberOfSpheres() << std::endl;
    std::cerr << "Hardness              : " << vfMsg->GetHardness() << std::endl;

    int n = vfMsg->GetNumberOfSpheres();

    for (int i = 0; i < n; i ++)
      {
      int res = 0;
      float x, y, z, r;
      res += vfMsg->GetCenter(i, x, y, z);
      res += vfMsg->GetRadius(i, r);
      if (res == 2*i)
        {
        std::cerr << "  Center[" << i << "] : (" << x << ", " << y << ", " << z << ")" << std::endl;
        std::cerr << "  Radius[" << i << "] : " << r << std::endl;
        }
      }
    return 1;
    }

  return 0;

}

int ReceiveScalarValue(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{

  std::cerr << "Receiving SCALARVALUE data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::ScalarValueMessage::Pointer svMsg;
  svMsg = igtl::ScalarValueMessage::New();
  svMsg->SetMessageHeader(header);
  svMsg->AllocatePack();
  
  // Receive transform data from the socket
  socket->Receive(svMsg->GetPackBodyPointer(), svMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = svMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    int size;
    int scalarType;
    unsigned char value[3];
    size = svMsg->GetDimension();
    scalarType = svMsg->GetScalarType();
    
    std::cerr << "Device Name: " << svMsg->GetDeviceName() << std::endl;
    std::cerr << "Size       : " << size << std::endl;
    std::cerr << "Scalar Type: " << scalarType << std::endl;
        
    memcpy(value, (unsigned char *)svMsg->GetScalarPointer(), igtl::ScalarValueMessage::ScalarSizeTable[scalarType] * size);
    for( int j = 0 ; j < size ; j ++ )
      {
      std::cerr << "Value No : " << j << std::endl;
      //std::cerr << "Value    : " << value[ j ] << std::endl;
      printf("%d \n",value[ j ]);
      }
    }

  return 0;

}
