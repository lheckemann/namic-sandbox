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

#include <math.h>
#include <time.h>


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

void LoopController::InitLoop(double interval_s, int n)
{
  this->m_StopFlag = 0;
  this->m_Interval = interval_s;

  this->m_TimeStamp->GetTime();
  this->m_StartTime = this->m_TimeStamp->GetTimeStamp();
  this->m_MaxCount  = n;

  this->m_PrevStartTime = 0.0;
  this->m_SimultaneousFlag = 0;
  
}

void LoopController::InitLoop(TimeTableType& timeTable)
{
  this->m_StopFlag = 0;
  this->m_Interval = -1.0;

  this->m_TimeStamp->GetTime();
  this->m_StartTime  = this->m_TimeStamp->GetTimeStamp();
  this->m_OriginTime = this->m_StartTime;

  this->m_Count = 0;
  this->m_TimeTable = timeTable;
  this->m_MaxCount  = timeTable.size();

  this->m_PrevStartTime = 0.0;
  this->m_SimultaneousFlag = 0;
}


int LoopController::StartLoop()
{
  // If the stop flag is ON or loop counter exceeds the limit
  if (this->m_StopFlag == 1 || this->m_Count >= this->m_MaxCount)
    {
    return 0;
    }

  if (this->m_Interval > 0)  // constant interval
    {
    // increment this->StartTime
    this->m_StartTime += this->m_Interval;
    }
  else                       // using TimeTable
    {
    this->m_StartTime = this->m_TimeTable[this->m_Count].time + this->m_OriginTime;
    this->m_Count ++;
    }

  // if the start time is same as the previous one, it doesn't need to sleep
  if (this->m_StartTime - this->m_PrevStartTime < 1e-5)
    {
    this->m_SimultaneousFlag = 1;
    }
  else
    {
    this->m_SimultaneousFlag = 0;

    // acquire current time
    this->m_TimeStamp->GetTime();
    
    // calculate time to sleep
    double tslp = this->m_StartTime - this->m_TimeStamp->GetTimeStamp();
    
    if (tslp > 0.0)
      {
      // convert from second to millisecond
      /*
        int tslp_ms = static_cast<int>(1000.0 * tslp);
        igtl::Sleep(tslp_ms);
      */
      
      // ---- POSIX specific code for high-resolution spleep
      struct timespec ts;
      struct timespec rm;
      ts.tv_sec  = static_cast<time_t>(tslp);
      ts.tv_nsec = static_cast<long>((tslp - floor(tslp)) * 1000000000);
      int r;
      do
        {
        r = nanosleep(&ts, &ts);
        }
      while (r == -1);
      // ---- end of POSIX specific code
      
      }
    }
  
  // calculate the delay
  this->m_TimeStamp->GetTime();
  this->m_Delay = this->m_TimeStamp->GetTimeStamp() - this->m_StartTime;

  this->m_PrevStartTime = this->m_StartTime;
  
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

double LoopController::GetScheduledTime()
{
  if (this->m_Count > 0)
    {
    //std::cerr << "Scheduled Time: " << (this->m_TimeTable[this->m_Count-1].time) << " -- " <<  
    //this->m_TimeTable[this->m_Count-1].type << std::endl;
    return this->m_TimeTable[this->m_Count-1].time + this->m_OriginTime;
    }
  else
    {
    return 0.0;
    }
}

} // namespace igtl

