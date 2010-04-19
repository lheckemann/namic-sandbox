/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlTimeStamp_h
#define __igtlTimeStamp_h

#include "igtlMacro.h"
#include "igtlObject.h"
#include "igtlObjectFactory.h"
#include "igtlTypes.h"

namespace igtl
{

class IGTLCommon_EXPORT TimeStamp : public Object
{
public:
  typedef TimeStamp                  Self;
  typedef Object                     Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  igtlTypeMacro(TimeStamp, Object);
  igtlNewMacro(Self);

  /** Returns the frequency of a clock */
  igtlGetConstMacro(Frequency,  igtlUint32);
  igtlGetConstMacro(Second,     igtlUint32);
  igtlGetConstMacro(Nanosecond, igtlUint32);

  void   GetTime();
  void   SetTime(double tm);
  void   SetTime(igtlUint32 second, igtlUint32 nanosecond);

  double GetTimeStamp();
  void   GetTimeStamp(igtlUint32* second, igtlUint32* nanosecond);

protected:

  /** constructor */
  TimeStamp();

  /** destructor */
  virtual ~TimeStamp();

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os) const;

private:

  igtlInt32       m_Frequency;   /* Clock frequency (Hz)*/
  igtlInt32       m_Second;      /* Second part of the time relative to 00:00:00 January 1, 1970 UTC */
  igtlInt32       m_Nanosecond;  /* Nano-second part of -- */


#if defined(WIN32) || defined(_WIN32)
  typedef double        TimeStampType;
  typedef double        FrequencyType;

  FrequencyType   m_WinFrequency;
  TimeStampType   m_WinDifference;
  TimeStampType   m_WinOrigin;
#endif

};

} // end of namespace igtl

#endif  // __igtlTimeStamp_h


