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

class IGTLCommon_EXPORT BindMessage: public MessageBase
{
public:
  typedef BindMessage                    Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::BindMessage, igtl::MessageBase);
  igtlNewMacro(igtl::BindMessage);

public:
  
  void        Init();

  int         SetNumberOfChildMessages(unsigned int n);
  int         GetNumberOfChildMessages();

  int         AppendChildMessage(igtl::MessageBase * child);
  int         SetChildMessage(unsigned int i, igtl::MessageBase * child);
  int         GetChildMessage(unsigned int i, igtl::MessageBase * ptr);

  const char* GetBind();

protected:
  BindMessage();
  ~BindMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  std::vector<igtl::MessageBase::Pointer> m_ChildMessages;
  
};


} // namespace igtl

#endif // _igtlBindMessage_h



