/*=========================================================================

  Program:   Open IGT Link -- Example for Data Receiving Client Program
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
#include "igtlClientSocket.h"

int ReceiveTransform(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header);
int ReceiveImage(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header);

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 3) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "    <hostname> : IP or host name"                    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    exit(0);
    }

  char*  hostname = argv[1];
  int    port     = atoi(argv[2]);

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
  // Create a message buffer to receive header
  igtl::MessageHeader::Pointer headerMsg;
  headerMsg = igtl::MessageHeader::New();
  
  while (1)
    {
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
      else
        {
        socket->Skip(headerMsg->GetBodySizeToRead(), 0);
        }
      }
    }

  //------------------------------------------------------------
  // Close connection (The example code never reachs to this section ...)
  
  socket->CloseSocket();

}


int ReceiveTransform(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
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
    }
}


int ReceiveImage(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
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
    }

}
