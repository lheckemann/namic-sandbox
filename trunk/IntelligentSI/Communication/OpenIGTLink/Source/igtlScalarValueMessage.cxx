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

#include "igtlScalarValueMessage.h"


#include "igtl_header.h"
#include "igtl_scalarvalue.h"

namespace igtl {

const int ScalarValueMessage::ScalarSizeTable[] = 
  {
    0, 0,
    1, 1,   // INT8 / UINT8
    2, 2,   // INT16 / UINT16
    4, 4,   // INT32 / UINT32
  };


ScalarValueMessage::ScalarValueMessage():
  MessageBase()
{


  endian        = ENDIAN_BIG;
    scalarType    = TYPE_UINT8;
  m_ScalarValueHeader = NULL;
  m_ScalarValue       = NULL;

  m_DefaultBodyType  = "SCALARVALUE";
}

ScalarValueMessage::~ScalarValueMessage()
{
}


  void ScalarValueMessage::SetDimension(int i)
  {
    dimension = i;
  }

  void ScalarValueMessage::GetDimension(int i)
  {
    i = dimension;
  }

void ScalarValueMessage::AllocateScalars()
{
  // Memory area to store scalarvalue scalar is allocated with
  // message and scalarvalue header, by using AllocatePack() implemented
  // in the parent class.
  AllocatePack();
  m_ScalarValueHeader = m_Body;
  m_ScalarValue  = &m_ScalarValueHeader[IGTL_SCALARVALUE_HEADER_SIZE];
}

void* ScalarValueMessage::GetScalarPointer()
{
  return (void*)m_ScalarValue;
}

int ScalarValueMessage::GetBodyPackSize()
{
  return GetScalarValueSize() + IGTL_SCALARVALUE_HEADER_SIZE;
}

int ScalarValueMessage::PackBody()
{
  igtl_scalarvalue_header* scalarvalue_header = (igtl_scalarvalue_header*)m_ScalarValueHeader;

  scalarvalue_header->version     = IGTL_SCALARVALUE_HEADER_VERSION;
  scalarvalue_header->scalar_type = this->scalarType;
  scalarvalue_header->endian      = this->endian;
  scalarvalue_header->dimension = dimension;
  igtl_scalarvalue_convert_byte_order(scalarvalue_header);

  return 1;

}


int ScalarValueMessage::UnpackBody()
{

  m_ScalarValueHeader = m_Body;

  igtl_scalarvalue_header* scalarvalue_header = (igtl_scalarvalue_header*)m_ScalarValueHeader;
  igtl_scalarvalue_convert_byte_order(scalarvalue_header);

  if (scalarvalue_header->version == IGTL_SCALARVALUE_HEADER_VERSION)
    {
      // ScalarValue format version 1
        this->scalarType       = scalarvalue_header->scalar_type;
      this->endian           = scalarvalue_header->endian;
      this->dimension = scalarvalue_header->dimension;

      m_ScalarValueHeader = m_Body;
      m_ScalarValue       = &m_ScalarValueHeader[IGTL_SCALARVALUE_HEADER_SIZE];
      
      return 1;
    }
  else
    {
      // Incompatible version. 
      return 0;
    }
}


} // namespace igtl



