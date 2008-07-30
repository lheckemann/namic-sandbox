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
  /** Standard class typedefs. */
  typedef LoopController            Self;
  typedef Object                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlNewMacro(Self);
  igtlTypeMacro(LoopController,Object);

  // InitLoop() initialize the member variables and
  // set the interval (second)
  void   InitLoop(double interval_s);
  
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

protected:
  LoopController();
  ~LoopController();


private:
  igtl::TimeStamp::Pointer m_TimeStamp;

  int    m_StopFlag;
  double m_Interval;
  double m_Delay;

  double m_StartTime;

};

}

#endif //__LoopController_h
