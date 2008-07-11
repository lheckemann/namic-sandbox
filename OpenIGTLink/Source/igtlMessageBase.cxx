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
#include "igtl_util.h"
#include "igtl_header.h"

namespace igtl {

MessageBase::MessageBase()
{
  m_PackSize       = 0;
  m_Header         = NULL;
  m_Body           = NULL;

  m_BodySizeToRead = 0;
  m_DeviceName = "";
}

MessageBase::~MessageBase()
{
  // TBD
}

int MessageBase::SetDeviceName(const char* name)
{
  m_DeviceName = std::string(name);
}

const char* MessageBase::GetDeviceName()
{
  return m_DeviceName.c_str();
}

const char* MessageBase::GetDeviceType()
{
  return m_BodyType.c_str();
}

int MessageBase::SetTimeStamp(unsigned int sec, unsigned int frac)
{
  m_TimeStampSec         = sec;
  m_TimeStampSecFraction = frac;
}

void MessageBase::Pack()
{
  PackBody();
  
  // pack header
  igtl_header* h = (igtl_header*) m_Header;

  igtl_uint64 crc = crc64(0, 0, 0LL); // initial crc

  h->version   = IGTL_HEADER_VERSION;

  igtl_uint64 ts  =  m_TimeStampSec & 0xFFFFFFFF;
  ts = ts << 32 || m_TimeStampSecFraction & 0xFFFFFFFF;

  h->timestamp = ts;
  h->body_size = GetBodyPackSize();
  h->crc       = crc64((unsigned char*)m_Body, GetBodyPackSize(), crc);

  strncpy(h->name, m_BodyType.c_str(), 12);
  strncpy(h->device_name, m_DeviceName.c_str(), 20);

  igtl_header_convert_byte_order(h);

}

void MessageBase::Unpack()
{
  // pack exists?
  if (m_Header != NULL && m_PackSize > IGTL_HEADER_SIZE)
    {
      igtl_header* h = (igtl_header*) m_Header;
      igtl_header_convert_byte_order(h);
      m_TimeStampSecFraction = h->timestamp & 0xFFFFFFFF;
      m_TimeStampSec = (h->timestamp >> 32 ) & 0xFFFFFFFF;
      m_BodySizeToRead = h->body_size;

      char bodyType[13];
      char deviceName[21];
      bodyType[12]   = '\0';
      deviceName[20] = '\0';
      strncpy(bodyType, h->name, 12);
      strncpy(deviceName, h->device_name, 20);

      m_BodyType = std::string(bodyType);
      m_DeviceName = std::string(deviceName);
    }
}

void* MessageBase::GetPackPointer()
{
  return (void*) m_Header;
}

void* MessageBase::GetPackBodyPointer()
{
  return (void*) m_Body;
}
 
int MessageBase::GetPackSize()
{
  return m_PackSize;
}

void MessageBase::AllocatePack()
{
  if (m_BodySizeToRead > 0)
    {
      AllocatePack(m_BodySizeToRead);
    }
  else
    {
      AllocatePack(GetBodyPackSize());
    }

//  int s = IGTL_HEADER_SIZE + GetBodyPackSize();
//  if (m_PackSize == 0 || m_Header == NULL)      // for the first time
//    {
//      m_Header = new unsigned char [s];
//    }
//  else if (m_PackSize != s)  // if the pack area exists but needs to be reallocated
//    {
//      unsigned char* old = m_Header;
//      m_Header = new unsigned char [s];
//      memcpy(m_Header, old, IGTL_HEADER_SIZE);
//      delete m_Header;
//    }
//  m_Body   = &m_Header[IGTL_HEADER_SIZE];
//  m_PackSize = s;
}

void MessageBase::AllocatePack(int bodySize)
{
  if (bodySize < 0)
    {
      bodySize = 0;
    }

  int s = IGTL_HEADER_SIZE + bodySize;
  if (m_PackSize == 0 || m_Header == NULL)      // for the first time
    {
      m_Header = new unsigned char [s];
    }
  else if (m_PackSize != s)  // if the pack area exists but needs to be reallocated
    {
      unsigned char* old = m_Header;
      m_Header = new unsigned char [s];
      memcpy(m_Header, old, IGTL_HEADER_SIZE);
      delete m_Header;
    }
  m_Body   = &m_Header[IGTL_HEADER_SIZE];
  m_PackSize = s;
}

int MessageBase::CopyHeader(const MessageBase &mb)
{

  this->m_PackSize       = mb.m_PackSize;
  this->m_BodySizeToRead = mb.m_BodySizeToRead;
  this->m_BodyType       = mb.m_BodyType;
  this->m_DeviceName     = mb.m_DeviceName;
  this->m_TimeStampSec   = mb.m_TimeStampSec;
  this->m_TimeStampSecFraction = mb.m_TimeStampSecFraction;

  memcpy(this->m_Header, mb.m_Header, IGTL_HEADER_SIZE);
  this->m_Body           = &this->m_Header[IGTL_HEADER_SIZE];

  return 1;
}

MessageBase& MessageBase::operator=(const MessageBase &mb)
{
  if (this == &mb)
    {
      return *this;
    }

  if (mb.m_PackSize > IGTL_HEADER_SIZE)
    {
      int bodySize = mb.m_PackSize - IGTL_HEADER_SIZE;
      this->AllocatePack(bodySize);
      this->CopyHeader(mb);
      if (bodySize > 0)
        {
          this->CopyBody(mb);
        }
    }
}

}



