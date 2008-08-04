/*=========================================================================

  Program:   Open IGT Link -- Example for Tracker Client Program
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __igtlLoopController_h
#define __igtlLoopController_h

#include "igtlObject.h"
#include "igtlObjectFactory.h"
#include "igtlMacro.h"

#include "igtlTimeStamp.h"

namespace igtl
{

class LoopController : public Object
{
public:
  typedef struct {
    int     type;
    double  time;
  } TimeTableElement;

  typedef std::vector<TimeTableElement> TimeTableType;

public:
  /** Standard class typedefs. */
  typedef LoopController            Self;
  typedef Object                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlNewMacro(Self);
  igtlTypeMacro(LoopController,Object);

  // InitLoop() initialize the member variables and
  // set the interval (second) and number of loops.
  // if n is set at 0, the loop becomes eternal loop.
  void   InitLoop(double interval_s, int n=0);

  void   InitLoop(TimeTableType& timeTable);
  
  // StartLoop() and function is palced at the begining
  // of the loop section. The function sleeps to adjust
  // the loop interval. StartLoop() returns 0, when the program
  // calls StopLoop() before calling StartLoop(). Otherwise
  // it returns 1. The function can be used with for(;;) loop as:
  //
  //    for (loop->InitLoop(0.01); loop->StartLoop();)
  //      {
  //      ...
  //      }
  //
  int    StartLoop();

  // StopLoop() function activate the flag to stop the loop.
  // After the function is called, StartLoop() returns NULL.
  int    StopLoop();

  // GetTimerDelay() returns the delay of StartLoop() function.
  // It is used to check the accuracy of sleeping duration in the
  // StartLoop() function.
  double GetTimerDelay();

  // GetEllapsedTime() returns time since the program finishes
  // calling the StartLoop() function.
  double GetEllapsedTime();

  // GetScheduledTime() returns time written on the time table.
  double GetScheduledTime();

  // Check if the scheduled time is same as the previous loop.
  int    IsSimultaneous()  { return this->m_SimultaneousFlag; };

  int    GetCount()  { return this->m_Count-1; };

protected:
  LoopController();
  ~LoopController();


private:
  igtl::TimeStamp::Pointer m_TimeStamp;

  int    m_StopFlag;
  double m_Interval;
  double m_Delay;

  double m_StartTime;
  double m_OriginTime;
  double m_PrevStartTime;
  int    m_SimultaneousFlag;
  int    m_MaxCount;
  int    m_Count;

  TimeTableType m_TimeTable;



};

}

#endif //__LoopController_h
