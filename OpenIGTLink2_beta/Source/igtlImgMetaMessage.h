/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink2_beta/Source/igtlImgmetaMessage.h $
  Language:  C++
  Date:      $Date: 2009-12-16 23:58:02 -0500 (Wed, 16 Dec 2009) $
  Version:   $Revision: 5466 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlImgMetaMessage_h
#define __igtlImgMetaMessage_h

#include <vector>
#include <string>

#include "igtlObject.h"
//#include "igtlMacros.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

#include "igtlImageMessage.h"

namespace igtl
{

class IGTLCommon_EXPORT ImgMetaElement: public Object
{
public:
  typedef ImgMetaElement                 Self;
  typedef Object                         Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::ImgMetaElement, igtl::Object);
  igtlNewMacro(igtl::ImgMetaElement);

public:
  int           SetName(const char* name);
  const char*   GetName()                            { return this->m_Name.c_str(); };

  int           SetDeviceName(const char* devname);
  const char*   GetDeviceName()                      { return this->m_DeviceName.c_str(); };

  int           SetModality(const char* modality);
  const char*   GetModality()                        { return this->m_Modality.c_str(); };

  int           SetPatientName(const char* patname);
  const char*   GetPatientName()                     { return this->m_PatientName.c_str(); };

  int           SetPatientID(const char* patid);
  const char*   GetPatientID()                       { return this->m_PatientID.c_str(); }

  void          SetTimeStamp(igtl::TimeStamp::Pointer& time);
  void          GetTimeStamp(igtl::TimeStamp::Pointer& time);
  //void          GetTimeStampUint64();

  void          SetSize(igtlUint16 size[3]);
  void          SetSize(igtlUint16 sx, igtlUint16 sy, igtlUint16 sz);
  void          GetSize(igtlUint16* size);
  void          GetSize(igtlUint16& sx, igtlUint16& sy, igtlUint16& sz);
  
  void          SetScalarType(igtlUint8 type);
  igtlUint8     GetScalarType();

protected:
  ImgMetaElement();
  ~ImgMetaElement();

protected:

  std::string   m_Name;        /* name / description (< 64 bytes)*/
  std::string   m_DeviceName;  /* device name to query the IMAGE and COLORT */
  std::string   m_Modality;    /* modality name (< 32 bytes) */
  std::string   m_PatientName; /* patient name (< 64 bytes) */ 
  std::string   m_PatientID;   /* patient ID (MRN etc.) (< 64 bytes) */  
  TimeStamp::Pointer m_TimeStamp;   /* scan time */
  igtlUint16    m_Size[3];     /* entire image volume size */ 
  igtlUint8     m_ScalarType;  /* scalar type. see scalar_type in IMAGE message */

};


class IGTLCommon_EXPORT GetImgMetaMessage: public MessageBase
{
protected:
  GetImgMetaMessage() : MessageBase() { this->m_DefaultBodyType  = "GET_IMGMETA"; };
  ~GetImgMetaMessage();
};


class IGTLCommon_EXPORT ImgMetaMessage: public MessageBase
{
public:
  typedef ImgMetaMessage                  Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::ImgMetaMessage, igtl::MessageBase);
  igtlNewMacro(igtl::ImgMetaMessage);

public:
  int AddImgMetaElement(ImgMetaElement::Pointer elem);
  int ClearImgMetaElement(ImgMetaElement::Pointer elem);

protected:
  ImgMetaMessage();
  ~ImgMetaMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  std::vector<ImgMetaElement::Pointer> m_ImgMetaList;
  
};


} // namespace igtl

#endif // _igtlImgMetaMessage_h



