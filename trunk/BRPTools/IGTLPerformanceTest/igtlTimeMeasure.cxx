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

#include <sys/times.h>
#include <iostream>

#include "igtlTimeMeasure.h"

namespace igtl
{

TimeMeasure::TimeMeasure()
{
  this->m_UserTime   = 0.0;
  this->m_SystemTime = 0.0;
  this->m_ActualTime = 0.0;
  this->m_TimeStamp  = igtl::TimeStamp::New();
}

TimeMeasure::~TimeMeasure()
{
}

void TimeMeasure::Start()
{
  this->m_TimeStamp->GetTime();
  times(&this->m_TmsStart);
}

void TimeMeasure::End()
{
  this->m_ActualTime = this->m_TimeStamp->GetTimeStamp();
  this->m_TimeStamp->GetTime();
  times(&this->m_TmsEnd);
  this->m_ActualTime = this->m_TimeStamp->GetTimeStamp() - this->m_ActualTime;
  this->m_SystemTime = ((double)(this->m_TmsEnd.tms_stime - this->m_TmsStart.tms_stime)) 
                        / (double)sysconf(_SC_CLK_TCK);
  this->m_UserTime   = ((double)(this->m_TmsEnd.tms_utime - this->m_TmsStart.tms_utime))
                        / (double)sysconf(_SC_CLK_TCK);
}

void TimeMeasure::PrintResult()
{
  std::cout.precision(10);
  std::cout << this->m_Name.c_str() << ", " << m_ActualTime << ", " << m_UserTime << ", " << m_SystemTime << std::endl;
}

void TimeMeasure::SetName(const char* name)
{
  this->m_Name = name;
}

}
