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

#ifndef __igtlMessageHeader_h
#define __igtlMessageHeader_h

#include "igtlObject.h"
#include "igtlObjectFactory.h"
//#include "igtlMacros.h"
#include "igtlMacro.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"

//-------------------------------------------------------------------------
// The MessageHeader class is used to receive and parse general headers
// to prepare for recieving body data. The class is currently just the alias
// of MessageBase class. Please refer igtlMessageBase.h for more details and
// the implementation of the class.
//
// The following is the typical unpacking (deserialization) prcedures
// using igtl::MessssageHeader class:
//
//     // Create instance and set Device Name
//     igtl::MessageBase::Pointer headerMsg;
//     headerMsg = igtl::MessageBase::New();
//
//     // Set up memory area to and receive the general header and unpack
//     headerMsg->AllocatePack();
//     socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
//     headerMsg->Unpack();
//
//     // Check data type string
//     if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM"))
//       {
//         igtl::TransformMessage::Pointer transMsg;
//         transMsg = igtl::TransformMessage::New();
//         transMsg->Copy(headerMsg);
//         transMsg->AllocatePack();
//         socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize());
//         transMsg->Unpack();
//       }
//     else if (strcmp(headerMsg->GetDeviceType(), "IMAGE"))
//       {
//         igtl::ImageMessage::Pointer imageMsg;
//         imageMsg = igtl::ImageMessage::New();
//         imageMsg->Copy(headerMsg);
//         imageMsg->AllocatePack();
//         socket->Receive(imageMsg->GetPackBodyPointer(), imageMsg->GetPackBodySize());
//         imageMsg->Unpack();
//       }
//     else if (...)
//       {
//          ...
//       }
//




namespace igtl
{

  typedef class MessageBase MessageHeader;

};


#endif //__igtlMessageHeader_h
