/*=========================================================================

  Program:   The OpenIGTLink Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink2_beta/Source/igtlBindMessage.cxx $
  Language:  C++
  Date:      $Date: 2010-01-17 13:04:20 -0500 (Sun, 17 Jan 2010) $
  Version:   $Revision: 5575 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlBindMessage.h"

#include "igtl_header.h"
#include "igtl_bind.h"

#include <string.h>

namespace igtl {

BindMessage::BindMessage():
  MessageBase()
{
  this->m_DefaultBodyType = "BIND";

  Init();
}


BindMessage::~BindMessage()
{
}

void BindMessage::Init()
{
  this->m_ChildMessages.clear();
}


int BindMessage::SetNumberOfChildMessages(unsigned int n)
{
  this->m_ChildMessages.resize(n);
  return this->m_ChildMessages.size();
}


int BindMessage::GetNumberOfChildMessages()
{
  return this->m_ChildMessages.size();
}


int BindMessage::AppendChildMessage(igtl::MessageBase * child)
{
  if (this->m_ChildMessages.size() < 0xFFFF)
    {
    ChildMessageInfo info;
    info.type = child->GetDeviceType();
    info.name = child->GetDeviceName();
    info.size = child->GetPackBodySize();
    info.ptr  = child->GetPackBodyPointer();
    this->m_ChildMessages.push_back(info);
    }
  return this->m_ChildMessages.size();
}


int BindMessage::SetChildMessage(unsigned int i, igtl::MessageBase * child)
{
  if (i < this->m_ChildMessages.size())
    {
    this->m_ChildMessages[i].type = child->GetDeviceType();
    this->m_ChildMessages[i].name = child->GetDeviceName();
    this->m_ChildMessages[i].size = child->GetPackBodySize();
    this->m_ChildMessages[i].ptr  = child->GetPackBodyPointer();
    return 1;
    }
  else
    {
    return 0;
    }
}


const char* BindMessage::GetChildMessageType(unsigned int i)
{
  if (i < this->m_ChildMessages.size())
    {
    this->m_ChildMessages[i].type.c_str();
    }
  else
    {
    return NULL;
    }
}


int BindMessage::GetChildMessage(unsigned int i, igtl::MessageBase * child)
{
  if (i < this->m_ChildMessages.size())
    {
    igtl_header * header = (igtl_header *) child->GetPackPointer();
    header->version = 1;
    strncpy( header->name, this->m_ChildMessages[i].type.c_str(),  IGTL_HEADER_TYPE_SIZE);
    strncpy( header->device_name, this->m_ChildMessages[i].name.c_str(), IGTL_HEADER_NAME_SIZE);

    // Time stamp -- same as the bind message
    igtl_uint64 ts  =  m_TimeStampSec & 0xFFFFFFFF;
    ts = (ts << 32) | (m_TimeStampSecFraction & 0xFFFFFFFF);
    header->timestamp = ts;
    header->body_size = this->m_ChildMessages[i].size;
    header->crc = 0;

    child->Unpack();
    child->AllocatePack();
    memcpy(child->GetPackBodyPointer(),
           this->m_ChildMessages[i].ptr, this->m_ChildMessages[i].size);

    return 1;
    }
  else
    {
    return 0;
    }
}

int BindMessage::GetBodyPackSize()
{
  int size;

  size = sizeof(igtlUint16)  // Number of child messages section
    + (IGTL_HEADER_TYPE_SIZE + sizeof(igtlUint64)) * this->m_ChildMessages.size() // BIND header
    + sizeof (igtlUint16);   // Size of name table section

  std::vector<ChildMessageInfo>::iterator iter;
  for (iter = this->m_ChildMessages.begin(); iter != this->m_ChildMessages.end(); iter ++)
    {
    //size += strlen((*iter)->GetDeviceName()); // Device name length
    size += (*iter).name.length();
    size += 1; // NULL separator
    size += (*iter).size;
    }

  // Body pack size is the sum of ENCODING, LENGTH and STRING fields
  return size;
}


int BindMessage::PackBody()
{
  // Allocate pack
  AllocatePack();

  igtl_bind_info bind_info;
  igtl_bind_init_info(&bind_info);
  igtl_bind_alloc_info(&bind_info, this->m_ChildMessages.size());

  // TODO: using c library causes additional data copy (C++ member variable to c-structure,
  // then to pack byte array). Probably, it's good idea to implement PackBody() without
  // using c APIs.
  int i = 0;
  std::vector<ChildMessageInfo>::iterator iter;
  for (iter = this->m_ChildMessages.begin(); iter != this->m_ChildMessages.end(); iter ++)
    {
    strncpy(bind_info.child_info_array[i].type, (*iter).type.c_str(), IGTL_HEADER_TYPE_SIZE);
    strncpy(bind_info.child_info_array[i].name, (*iter).name.c_str(), IGTL_HEADER_NAME_SIZE);
    bind_info.child_info_array[i].size = (*iter).size;
    bind_info.child_info_array[i].ptr = (*iter).ptr;
    i ++;
    }

  igtl_bind_pack(&bind_info, this->m_Body);
  igtl_bind_free_info(&bind_info);

  return 1;
}

int BindMessage::UnpackBody()
{

  igtl_bind_info bind_info;

  if (igtl_bind_unpack((void*)this->m_Body, &bind_info) == 0)
    {
    return 0;
    }

  int n = bind_info.ncmessages;

  Init();

  for (int i = 0; i < n; i ++)
    {
    ChildMessageInfo info;

    info.type = bind_info.child_info_array[i].type;
    info.name = bind_info.child_info_array[i].name;
    info.size = bind_info.child_info_array[i].size;
    info.ptr  = bind_info.child_info_array[i].ptr;

    this->m_ChildMessages.push_back(info);
    }

  return 1;
}

} // namespace igtl





