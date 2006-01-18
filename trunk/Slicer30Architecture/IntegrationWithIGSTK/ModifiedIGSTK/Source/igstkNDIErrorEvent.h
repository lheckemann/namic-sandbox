/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkNDIErrorEvent.h,v $
  Language:  C++
  Date:      $Date: 2005/09/25 15:40:05 $
  Version:   $Revision: 1.2 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __igstkNDIErrorEvent_H
#define __igstkNDIErrorEvent_H


#include "igstkEvents.h"


namespace igstk
{

/** \class NDIErrorEvent
 *  \brief Report errors for the NDICommandInterpreter
 *
 *  The NDIErrorEvent is generated whenever NDICommandInterpreter
 *  encounters an error while communicating with an NDI tracking
 *  device.
 */

class IGSTK_EXPORT NDIErrorEvent : public IGSTKEvent 
{

public: 
  typedef NDIErrorEvent Self; 
  typedef IGSTKEvent Superclass;

  /** Constructor */
  NDIErrorEvent() {
    m_ErrorCode = 0; };

  /** Constructor with error code */
  NDIErrorEvent( int errorCode ) {
    m_ErrorCode = errorCode; };

  /** Copy constructor */
  NDIErrorEvent(const Self & s) : IGSTKEvent( s ) {
    m_ErrorCode = s.m_ErrorCode; };
 
  /** Destructor */
  virtual ~NDIErrorEvent() {};

  /** Get the name of this event */
  virtual const char * GetEventName() const {
    return "NDIErrorEvent"; };

  /** check whether an event derives from this event */
  virtual bool CheckEvent(const ::itk::EventObject* e) const {
    return dynamic_cast<const Self*>(e); };

  /** Make another event of the same type */
  virtual ::itk::EventObject* MakeObject() const {
    return new Self; }

  /** Get the error code */
  const unsigned int GetErrorCode() const {
    return m_ErrorCode; };

private:
  void operator=(const Self&); 

  // Payload of this event.
  int  m_ErrorCode;
};


}

#endif

