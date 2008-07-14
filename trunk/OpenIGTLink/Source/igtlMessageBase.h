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

#include "igtlMessageHeader.h"

//-------------------------------------------------------------------------
// The MessageBase class is the base class of all message type classes
// used in the Open IGT Link Library. The message classes can be used
// both for serializing (packing) Open IGT Link message byte streams.
// The class can also deserializing (unpacking) Open IGT Link.
// For the deserialization example, please refer igtlMessageHeader.h.
// 
// The typical packing procedures using sub-classes of
// MessageBase look like the followings
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
  int   GetPackBodySize();

  const char* GetBodyType() { return this->m_BodyType.c_str(); };

  // Allocate memory for packing / receiving buffer
  void AllocatePack();

  // Call InitPack() before receive header. 
  // This function simply reset the Unpacked flag for both
  // the hearder and body pack.
  void InitPack();

  // Copy contents from the specified Massage class.
  // If the type of the specified class is same as this class,
  // both general header and body are copied. Otherwise, only
  // general header is copied.
  int Copy(const MessageBase* mb);

  int SetMessageHeader(const MessageHeader* mb) { Copy(mb); };
  
protected:
  MessageBase();
  ~MessageBase();

protected:

  // Pack body (must be implemented in a child class)
  virtual int  GetBodyPackSize() { return 0; };
  virtual void PackBody()        {};
  virtual void UnpackBody()      {};

  // Allocate memory specifying the body size
  // (used when create a brank package to receive data)
  void AllocatePack(int bodySize);

  // Copy data 
  int CopyHeader(const MessageBase *mb);
  virtual int CopyBody(const MessageBase *mb) { return 0; };


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

  // Unpacking status (0: --   1: unpacked)
  int            m_IsHeaderUnpacked;
  int            m_IsBodyUnpacked;

};


} // namespace igtl

#endif // _igtlMessageBase_h


