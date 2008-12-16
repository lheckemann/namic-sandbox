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
#include "igtl_vfixture.h"

namespace igtl {

ScalarValueMessage::ScalarValueMessage():
  MessageBase()
{
  m_NSpheres = 0;
  m_Hardness = 0.0;
  m_CenterX  = NULL;
  m_CenterY  = NULL;
  m_CenterZ  = NULL;
  m_Radius   = NULL;
  m_ScalarValueHeader = NULL;
  m_ScalarValue       = NULL;
  m_DefaultBodyType  = "*VFIXTURE";
}

ScalarValueMessage::~ScalarValueMessage()
{
  if (m_CenterX != NULL)
    {
    delete [] m_CenterX;
    }
  if (m_CenterY != NULL)
    {
    delete [] m_CenterY;
    }
  if (m_CenterZ != NULL)
    {
    delete [] m_CenterZ;
    }
  if (m_Radius != NULL)
    {
    delete [] m_Radius;
    }
}

void  ScalarValueMessage::SetNumberOfSpheres(int n)
{
  if (m_NSpheres != n)
    {
    if (m_CenterX != NULL)
      {
      delete [] m_CenterX;
      }
    if (m_CenterY != NULL)
      {
      delete [] m_CenterY;
      }
    if (m_CenterZ != NULL)
      {
      delete [] m_CenterZ;
      }
    if (m_Radius != NULL)
      {
      delete [] m_Radius;
      }
    m_NSpheres = n;
    m_CenterX  = new float[n];
    m_CenterY  = new float[n];
    m_CenterZ  = new float[n];
    m_Radius   = new float[n];
    for (int i = 0; i ++; i < n)
      {
      m_CenterX[i] = 0.0;
      m_CenterY[i] = 0.0;
      m_CenterZ[i] = 0.0;
      m_Radius[i]  = 0.0;
      }
    }
}

int ScalarValueMessage::GetNumberOfSpheres()
{
  return m_NSpheres;
}

void ScalarValueMessage::SetHardness(float h)
{
  m_Hardness = h;
}

float ScalarValueMessage::GetHardness()
{
  return m_Hardness;
}

int ScalarValueMessage::SetCenter(int id, float x, float y, float z)
{
  if (id >= 0 && id < m_NSpheres)
    {
    m_CenterX[id] = x;
    m_CenterY[id] = y;
    m_CenterZ[id] = z;
    return id;
    }
  else
    {
    return -1;
    }
}

int ScalarValueMessage::GetCenter(int id, float& x, float& y, float& z)
{
  if (id >= 0 && id < m_NSpheres)
    {
    x = m_CenterX[id];
    y = m_CenterY[id];
    z = m_CenterZ[id];
    return id;
    }
  else
    {
    return -1;
    }
}

int ScalarValueMessage::SetRadius(int id, float r)
{
  if (id >= 0 && id < m_NSpheres)
    {
    m_Radius[id] = r;
    return id;
    }
  else
    {
    return -1;
    }
}

int ScalarValueMessage::GetRadius(int id, float& r)
{
  if (id >= 0 && id < m_NSpheres)
    {
    r = m_Radius[id];
    return id;
    }
  else
    {
    return -1;
    }
}

int ScalarValueMessage::GetDataSize()
{
  return m_NSpheres * sizeof(igtl_vfixture_data);
}

void ScalarValueMessage::AllocateScalars()
{
  // Memory area to store image scalar is allocated with
  // message and image header, by using AllocatePack() implemented
  // in the parent class.
  AllocatePack();
  m_ScalarValueHeader = m_Body;
  m_ScalarValue  = &m_ScalarValueHeader[IGTL_VFIXTURE_HEADER_SIZE];
}

void* ScalarValueMessage::GetScalarPointer()
{
  return (void*)m_ScalarValue;
}

int ScalarValueMessage::GetBodyPackSize()
{
  return GetDataSize() + IGTL_VFIXTURE_HEADER_SIZE;
}

int ScalarValueMessage::PackBody()
{
  igtl_vfixture_header* header = (igtl_vfixture_header*)m_ScalarValueHeader;

  // Copy common parameters
  header->version  = IGTL_VFIXTURE_HEADER_VERSION;
  header->nspheres = m_NSpheres;
  header->hardness = m_Hardness;

  // Copy sphere data
  for (int i = 0; i < m_NSpheres; i ++)
    {
    igtl_vfixture_data* data = (igtl_vfixture_data*)&m_ScalarValue[sizeof(igtl_vfixture_data)*i];
    data->x = m_CenterX[i];
    data->y = m_CenterY[i];
    data->z = m_CenterZ[i];
    data->r = m_Radius[i];
    }

  igtl_vfixture_convert_byte_order(header);
  igtl_vfixture_data_convert_byte_order(m_NSpheres, m_ScalarValue);

  return 1;
}


int ScalarValueMessage::UnpackBody()
{

  m_ScalarValueHeader = m_Body;

  igtl_vfixture_header* header = (igtl_vfixture_header*)m_ScalarValueHeader;
  igtl_vfixture_convert_byte_order(header);

  if (header->version == IGTL_VFIXTURE_HEADER_VERSION)
    {
    // ScalarValue format version 1
    SetNumberOfSpheres(header->nspheres);
    m_Hardness = header->hardness;

    m_ScalarValueHeader = m_Body;
    m_ScalarValue       = &m_ScalarValueHeader[IGTL_VFIXTURE_HEADER_SIZE];

    igtl_vfixture_data_convert_byte_order(m_NSpheres, m_ScalarValue);
    for (int i = 0; i < m_NSpheres; i ++)
      {
      igtl_vfixture_data* data = (igtl_vfixture_data*)&m_ScalarValue[sizeof(igtl_vfixture_data)*i];
      SetCenter(i, data->x, data->y, data->z);
      SetRadius(i, data->r);
      }
    return 1;
    }
  else
    {
      // Incompatible version. 
      return 0;
    }
}


} // namespace igtl



