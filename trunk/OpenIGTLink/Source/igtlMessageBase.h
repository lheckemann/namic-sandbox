/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlMessageBase_h
#define __igtlMessageBase_h

#include "igtlObject.h"
#include "igtlObjectFactory.h"
//#include "igtlMacros.h"
#include "igtlMacro.h"
#include "igtlMath.h"

//-------------------------------------------------------------------------
// The MessageBase class is the base class of all message type classes
// used in the Open IGT Link Library. The message classes can be used
// both for serializing (packing) Open IGT Link message byte streams and
// deserializing (unpacking) Open IGT Link.
// 
// The typical packing/unpacking procedures using sub-classes of
// MessageBase look like the followings
//
//  a) Packing (ex. TransformMessage class)
//
//     // Create instance and set Device Name
//     igtl::TransformMessage::Pointer transMsg;
//     transMsg = igtl::TransformMessage::New();
//     transMsg->SetDeviceName("Tracker");
//
//     // Create matrix and substitute values
//     igtl::Matrix4x4 matrix;
//     GetRandomTestMatrix(matrix);
//
//     // Set matrix data, serialize, and send it.
//     transMsg->SetMatrix(matrix);
//     transMsg->Pack();
//     socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());
//
//  b) Unpacking
//
//     // Create instance and set Device Name
//     igtl::MessageBase::Pointer headerMsg;
//     headerMsg = igtl::MessageBase::New();
//
//     // Set up memory area to and receive the general header and unpack
//     headerMsg->AllocatePack();
//     socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
//     headerMsg->Unpack();
//
//     // Check data type string
//     if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM"))
//       {
//         igtl::TransformMessage::Pointer transMsg;
//         transMsg = igtl::TransformMessage::New();
//         transMsg = headerMsg;
//         transMsg->AllocatePack();
//         socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackSize());
//         transMsg->Unpack();
//       }
//     else if (strcmp(headerMsg->GetDeviceType(), "IMAGE"))
//       {
//         igtl::ImageMessage::Pointer imageMsg;
//         imageMsg = igtl::ImageMessage::New();
//         imageMsg = headerMsg;
//         imageMsg->AllocatePack();
//         socket->Receive(imageMsg->GetPackBodyPointer(), imageMsg->GetPackSize());
//         imageMsg->Unpack();
//       }
//     else if (...)
//       {
//          ...
//       }
//
//-------------------------------------------------------------------------

namespace igtl
{

class MessageBase: public Object
{
public:
  typedef MessageBase               Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::MessageBase, igtl::Object)
  igtlNewMacro(igtl::MessageBase);

public:

  void  Delete() { delete this; };

  int   SetDeviceName(const char* name);
  const char* GetDeviceName();
  const char* GetDeviceType();
  
  int   SetTimeStamp(unsigned int sec, unsigned int frac);
  int   GetTimeStamp(unsigned int* sec, unsigned int* frac);

  void  Pack();
  void  Unpack();
  void* GetPackPointer();
  void* GetPackBodyPointer();
  int   GetPackSize();

  const char* GetBodyType() { return this->m_BodyType.c_str(); };

  // Allocate memory for packing / receiving buffer
  void AllocatePack();
  
  // The '=' operator is defined to substitute base class to
  // a child message class, after receiving the general header.
  MessageBase& operator=(const MessageBase &mb);

protected:
  MessageBase();
  ~MessageBase();

protected:

  // Pack body (must be implemented in a child class)
  virtual int  GetBodyPackSize() { return 0; };
  virtual void PackBody()        {};

  // Allocate memory specifying the body size
  // (used when create a brank package to receive data)
  void AllocatePack(int bodySize);

  // Copy data (used for operator=)
  int CopyHeader(const MessageBase &mb);
  virtual int CopyBody(const MessageBase &mb) { return 0; };

  // Pointers to header and image
  //  To prevent large copy of byte array in Pack() function,
  //  header byte array is concatinated to that of image.
  //  Consequently,
  //    body = header + sizeof (igtl_header)
  //  after these areas are allocated.
  //
  int            m_PackSize;
  unsigned char* m_Header;
  unsigned char* m_Body;

  int            m_BodySizeToRead;

  //BTX
  std::string    m_BodyType;
  std::string    m_DeviceName;
  //ETX
  unsigned int   m_TimeStampSec;
  unsigned int   m_TimeStampSecFraction;

};


} // namespace igtl

#endif // _igtlMessageBase_h


