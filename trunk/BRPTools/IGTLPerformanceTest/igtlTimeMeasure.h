/*=========================================================================

  Program:   Open IGT Link -- Performance Test Prgram
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// -- NOTE
// Currently this class can be used only on POSIX environment
//

#ifndef __igtlTimeMeasure_h
#define __igtlTimeMeasure_h

#include "igtlObject.h"
#include "igtlObjectFactory.h"
#include "igtlMacro.h"

#include "igtlTimeStamp.h"

#include <string>
#include <sys/times.h>

namespace igtl
{

class TimeMeasure : public Object
{
public:
  /** Standard class typedefs. */
  typedef TimeMeasure               Self;
  typedef Object                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlNewMacro(Self);
  igtlTypeMacro(TimeMeasure,Object);

  void Start();
  void End();

  void PrintResult();

  double GetUserTime()   {return m_UserTime; };
  double GetSystemTime() {return m_SystemTime; };
  double GetActualTime() {return m_ActualTime; };

  void SetName(const char* name);
  const char* GetName()  {return m_Name.c_str(); };

protected:
  TimeMeasure();
  ~TimeMeasure();

private:

  double  m_UserTime;
  double  m_SystemTime;
  double  m_ActualTime;

  igtl::TimeStamp::Pointer m_TimeStamp;

  struct tms m_TmsStart;
  struct tms m_TmsEnd;

  //BTX
  std::string m_Name;
  //ETX

};

}

#endif //__igtlTimeMeasure_h
