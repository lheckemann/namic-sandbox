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

#include "igtlVFixtureMessage.h"

#include "igtl_header.h"
#include "igtl_vfixture.h"

namespace igtl {

VFixtureMessage::VFixtureMessage():
  MessageBase()
{
  m_NSpheres = 0;
  m_Hardness = 0.0;
  m_CenterX  = NULL;
  m_CenterY  = NULL;
  m_CenterZ  = NULL;
  m_Radius   = NULL;
  m_VFixtureHeader = NULL;
  m_VFixture       = NULL;
  m_DefaultBodyType  = "*VFIXTURE";
}

VFixtureMessage::~VFixtureMessage()
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

void  VFixtureMessage::SetNumberOfSpheres(int n)
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

int VFixtureMessage::GetNumberOfSpheres()
{
  return m_NSpheres;
}

void VFixtureMessage::SetHardness(float h)
{
  m_Hardness = h;
}

float VFixtureMessage::GetHardness()
{
  return m_Hardness;
}

int VFixtureMessage::SetCenter(int id, float x, float y, float z)
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

int VFixtureMessage::GetCenter(int id, float& x, float& y, float& z)
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

int VFixtureMessage::SetRadius(int id, float r)
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

int VFixtureMessage::GetRadius(int id, float& r)
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

int VFixtureMessage::GetDataSize()
{
  return m_NSpheres * sizeof(igtl_vfixture_data);
}

void VFixtureMessage::AllocateScalars()
{
  // Memory area to store image scalar is allocated with
  // message and image header, by using AllocatePack() implemented
  // in the parent class.
  AllocatePack();
  m_VFixtureHeader = m_Body;
  m_VFixture  = &m_VFixtureHeader[IGTL_VFIXTURE_HEADER_SIZE];
}

void* VFixtureMessage::GetScalarPointer()
{
  return (void*)m_VFixture;
}

int VFixtureMessage::GetBodyPackSize()
{
  return GetDataSize() + IGTL_VFIXTURE_HEADER_SIZE;
}

int VFixtureMessage::PackBody()
{
  igtl_vfixture_header* header = (igtl_vfixture_header*)m_VFixtureHeader;

  // Copy common parameters
  header->version  = IGTL_VFIXTURE_HEADER_VERSION;
  header->nspheres = m_NSpheres;
  header->hardness = m_Hardness;

  // Copy sphere data
  for (int i = 0; i < m_NSpheres; i ++)
    {
    igtl_vfixture_data* data = (igtl_vfixture_data*)&m_VFixture[sizeof(igtl_vfixture_data)*i];
    data->x = m_CenterX[i];
    data->y = m_CenterY[i];
    data->z = m_CenterZ[i];
    data->r = m_Radius[i];
    }

  igtl_vfixture_convert_byte_order(header);
  igtl_vfixture_data_convert_byte_order(m_NSpheres, m_VFixture);

  return 1;
}


int VFixtureMessage::UnpackBody()
{

  m_VFixtureHeader = m_Body;

  igtl_vfixture_header* header = (igtl_vfixture_header*)m_VFixtureHeader;
  igtl_vfixture_convert_byte_order(header);

  if (header->version == IGTL_VFIXTURE_HEADER_VERSION)
    {
    // VFixture format version 1
    SetNumberOfSpheres(header->nspheres);
    m_Hardness = header->hardness;

    m_VFixtureHeader = m_Body;
    m_VFixture       = &m_VFixtureHeader[IGTL_VFIXTURE_HEADER_SIZE];

    igtl_vfixture_data_convert_byte_order(m_NSpheres, m_VFixture);
    for (int i = 0; i < m_NSpheres; i ++)
      {
      igtl_vfixture_data* data = (igtl_vfixture_data*)&m_VFixture[sizeof(igtl_vfixture_data)*i];
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



