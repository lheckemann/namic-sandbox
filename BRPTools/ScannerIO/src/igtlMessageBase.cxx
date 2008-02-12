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

#include <string.h>
#include "igtlMessageBase.h"
#include "igtl_header.h"
#define PROTOTYPES
#include "crc32.h"

namespace igtl {

MessageBase::MessageBase()
{
  m_PackSize   = 0;
  m_Header     = NULL;
  m_Body       = NULL;

  m_DeviceName = "";
}

MessageBase::~MessageBase()
{
  // TBD
}

int MessageBase::SetDeviceName(std::string name)
{
  m_DeviceName = name;
}

void MessageBase::Pack()
{
  PackBody();
  
  // pack header
  igtl_header* h = (igtl_header*) m_Header;

  int crc = crc32(0L, Z_NULL, 0); // initial crc

  h->version = IGTL_HEADER_VERSION;
  h->timestamp = 0;
  h->body_size = GetBodyPackSize();
  h->crc       = crc32(crc, m_Body, GetBodyPackSize());  // calculate crc

  strncpy(h->name, m_BodyType.c_str(), 8);
  strncpy(h->device_name, m_DeviceName.c_str(), 20);

  igtl_header_convert_byte_order(h);

}

void* MessageBase::GetPackPointer()
{
  return (void*) m_Header;
}
 
int MessageBase::GetPackSize()
{
  return m_PackSize;
}

void MessageBase::AllocatePack()
{
  int s = IGTL_HEADER_SIZE + GetBodyPackSize();
  if (m_PackSize != s)
    {
      if (m_Header != NULL)
        {
          delete m_Header;
        }
      m_Header = new unsigned char [s];
      m_Body   = &m_Header[IGTL_HEADER_SIZE];
      m_PackSize = s;
    }
}
}



