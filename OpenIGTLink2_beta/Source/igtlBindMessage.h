/*=========================================================================

  Program:   The OpenIGTLink Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink2_beta/Source/igtlBindMessage.h $
  Language:  C++
  Date:      $Date: 2009-12-16 23:58:02 -0500 (Wed, 16 Dec 2009) $
  Version:   $Revision: 5466 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlBindMessage_h
#define __igtlBindMessage_h

#include <string>

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

namespace igtl
{
//
// Base class for BindMessage, GetBindMessage and StartBindMessage
//
class IGTLCommon_EXPORT BindMessageBase: public MessageBase
{
public:
  typedef BindMessageBase                Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::BindMessageBase, igtl::MessageBase);
  igtlNewMacro(igtl::BindMessageBase);

public:
  
  void        Init();

  int         SetNumberOfChildMessages(unsigned int n);
  int         GetNumberOfChildMessages();

  int         AppendChildMessage(igtl::MessageBase * child);
  int         SetChildMessage(unsigned int i, igtl::MessageBase * child);

  const char* GetChildMessageType(unsigned int i);

protected:

  BindMessageBase();
  ~BindMessageBase();
  
protected:

  typedef struct {
    std::string  type;
    std::string  name;
    igtlUint64   size;
    void *       ptr;
  } ChildMessageInfo;

  std::vector<ChildMessageInfo> m_ChildMessages;
  
};


class IGTLCommon_EXPORT BindMessage: public BindMessageBase
{
public:
  typedef BindMessage                    Self;
  typedef BindMessageBase                Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::BindMessage, igtl::BindMessageBase);
  igtlNewMacro(igtl::BindMessage);

public:
  
  int         GetChildMessage(unsigned int i, igtl::MessageBase * child);

protected:
  BindMessage();
  ~BindMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

};


class IGTLCommon_EXPORT GetBindMessage: public BindMessageBase
{
public:
  typedef GetBindMessage               Self;
  typedef BindMessageBase                Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::GetBindMessage, igtl::BindMessageBase);
  igtlNewMacro(igtl::GetBindMessage);

public:
  
  int         AppendChildMessage(const char * type, const char * name);

protected:
  GetBindMessage();
  ~GetBindMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

};


} // namespace igtl

#endif // _igtlBindMessage_h



