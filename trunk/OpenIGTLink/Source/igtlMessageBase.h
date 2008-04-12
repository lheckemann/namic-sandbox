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

  int   SetDeviceName(std::string name);

  void  Pack();
  void* GetPackPointer();
  int   GetPackSize();

  const char* GetBodyType() { return this->m_BodyType.c_str(); };

protected:
  MessageBase();
  ~MessageBase();

protected:

  // Pack body (must be implemented in a child class)
  virtual int GetBodyPackSize() { return 0; };
  virtual void PackBody()       {};

  // Allocate memory for packing
  void AllocatePack();

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

  std::string    m_BodyType;
  std::string    m_DeviceName;

};


} // namespace igtl

#endif // _igtlMessageBase_h


