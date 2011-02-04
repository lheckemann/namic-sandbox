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
    this->m_ChildMessages.push_back(child);
    }
  return this->m_ChildMessages.size();
}


int BindMessage::SetChildMessage(unsigned int i, igtl::MessageBase * child)
{
  if (i < this->m_ChildMessages.size())
    {
    this->m_ChildMessages[i] = child;
    return 1;
    }
  else
    {
    return 0;
    }
}


int BindMessage::GetChildMessage(unsigned int i, igtl::MessageBase * ptr)
{
  if (i < this->m_ChildMessages.size())
    {
    ptr = this->m_ChildMessages[i];
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

  std::vector<igtl::MessageBase::Pointer>::iterator iter;
  for (iter = this->m_ChildMessages.begin(); iter != this->m_ChildMessages.end(); iter ++)
    {
    size += strlen((*iter)->GetDeviceName()); // Device name length
    size += 1; // NULL separator
    size += (*iter)->GetPackBodySize();
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
  std::vector<igtl::MessageBase::Pointer>::iterator iter;
  for (iter = this->m_ChildMessages.begin(); iter != this->m_ChildMessages.end(); iter ++)
    {
    strncpy(bind_info.child_info_array[i].type, (*iter)->GetDeviceType(), IGTL_HEADER_TYPE_SIZE);
    strncpy(bind_info.child_info_array[i].name, (*iter)->GetDeviceName(), IGTL_HEADER_NAME_SIZE);
    bind_info.child_info_array[i].size = (*iter)->GetPackBodySize();
    bind_info.child_info_array[i].ptr = (*iter)->GetPackBodyPointer();
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
    igtl::MessageHeader::Pointer msgHeader;
    msgHeader = igtl::MessageHeader::New();

    msgHeader->InitPack();
    
    igtl_header * header = (igtl_header *) msgHeader->GetPackPointer();
    header->version = 1;
    strncpy( header->name, bind_info.child_info_array[i].type,  IGTL_HEADER_TYPE_SIZE);
    strncpy( header->device_name, bind_info.child_info_array[i].name, IGTL_HEADER_NAME_SIZE);
    header->timestamp = 0;
    header->body_size = bind_info.child_info_array[i].size;
    header->crc = 0;
    msgHeader->Unpack();

    igtl::MessageBase::Pointer msg;
    msg = igtl::MessageBase::New();
    msg->SetMessageHeader(msgHeader);
    msg->AllocatePack();

    // TODO: Is there any way to avoid this memory copy?
    memcpy(msg->GetPackBodyPointer(), bind_info.child_info_array[i].ptr,
           bind_info.child_info_array[i].size);

    msg->Unpack();
    
    this->m_ChildMessages.push_back(msg);
    }

  return 1;
}

} // namespace igtl





