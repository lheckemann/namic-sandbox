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

#include "igtlLoopController.h"
#include "igtlTimeStamp.h"
#include "igtlOSUtil.h"

namespace igtl
{


LoopController::LoopController()
{
  this->m_StopFlag  = 0;
  this->m_Delay     = 0.0;
  this->m_Interval  = 0.0;
  this->m_StartTime = 0.0;

  this->m_TimeStamp   = igtl::TimeStamp::New();
}

LoopController::~LoopController()
{
}

void LoopController::InitLoop(double interval_s)
{
  this->m_StopFlag = 0;
  this->m_Interval = interval_s;

  this->m_TimeStamp->GetTime();
  this->m_StartTime = this->m_TimeStamp->GetTimeStamp();
}


int LoopController::StartLoop()
{

  // If the stop flag is ON
  if (this->m_StopFlag == 1)
    {
    return 0;
    }

  // increment this->StartTime
  this->m_StartTime += this->m_Interval;

  // acquire current time
  this->m_TimeStamp->GetTime();

  // calculate time to sleep
  double tslp = this->m_StartTime - this->m_TimeStamp->GetTimeStamp();

  if (tslp > 0.0)
    {
    // convert from second to millisecond
    int tslp_ms = static_cast<int>(1000.0 * tslp);
    igtl::Sleep(tslp_ms);
    }

  // calculate the delay
  this->m_TimeStamp->GetTime();
  this->m_Delay = this->m_TimeStamp->GetTimeStamp() - this->m_StartTime;

  return 1;
}

int LoopController::StopLoop()
{
  this->m_StopFlag = 1;
  return 1;
}

double LoopController::GetTimerDelay()
{
  return this->m_Delay;
}

double LoopController::GetEllapsedTime()
{
  // acquire current time
  this->m_TimeStamp->GetTime();

  // calculate time to sleep
  double ellapsed = this->m_TimeStamp->GetTimeStamp() - this->m_StartTime;

  return ellapsed;
}

}
