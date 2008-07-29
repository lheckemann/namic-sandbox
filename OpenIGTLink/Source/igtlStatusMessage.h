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

#ifndef __igtlStatusMessage_h
#define __igtlStatusMessage_h

#include "igtlObject.h"
//#include "igtlMacros.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

namespace igtl
{

class StatusMessage: public MessageBase
{
public:
  typedef StatusMessage                  Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::StatusMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StatusMessage);

public:

  /* Status codes -- see igtl_status.h */
  enum {
  };

public:

  void    SetCode(int code);
  int     GetSubCode(igtlInt64 subcode);

  void    Set
  
  void    SetStatusString(const char* message);

protected:
  StatusMessage();
  ~StatusMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  Matrix4x4 matrix;

  unsigned char*  m_Transform;

};


} // namespace igtl

#endif // _igtlStatusMessage_h



