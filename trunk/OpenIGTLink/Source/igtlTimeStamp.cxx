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


/*=========================================================================

  Part of the code is copied from itk::RealTimeClock class in
  Insight Segmentation & Registration Toolkit:


  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkRealTimeClock.cxx,v $
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlTimeStamp.h"

#include <math.h>

#if defined(WIN32) || defined(_WIN32)
  #include <windows.h>
#else
  #include <sys/time.h>
#endif  // defined(WIN32) || defined(_WIN32)

#include <string.h>

#include "igtl_util.h"

namespace igtl
{

TimeStamp::TimeStamp(): Object()
{
#if defined(WIN32) || defined(_WIN32)

  //// The following code is deactiveted tempolorary due to unexpected behavior
  //// on 64-bit platform

  //LARGE_INTEGER frequency;
  //::QueryPerformanceFrequency(&frequency);
  //
  //this->m_WinFrequency = 
  //  static_cast< FrequencyType >( (__int64)frequency.QuadPart );
  //
  //SYSTEMTIME st1;
  //SYSTEMTIME st2;
  //FILETIME ft1;
  //FILETIME ft2;
  //
  //::memset( &st1, 0, sizeof( st1 ) );
  //::memset( &st2, 0, sizeof( st2 ) );
  //
  //st1.wYear = 1601;
  //st1.wMonth = 1;
  //st1.wDay = 1;
  //
  //st2.wYear = 1970;
  //st2.wMonth = 1;
  //st2.wDay = 1;
  //
  //::SystemTimeToFileTime(&st1, &ft1);
  //::SystemTimeToFileTime(&st2, &ft2);
  //
  //LARGE_INTEGER ui1;
  //LARGE_INTEGER ui2;
  //
  //// The following code didn't work on Win64 due to the difference
  //// of memory boundary
  ////memcpy( &ui1, &ft1, sizeof( ui1 ) );
  ////memcpy( &ui2, &ft2, sizeof( ui2 ) );
  //ui1.LowPart  = ft1.dwLowDateTime;
  //ui1.HighPart = ft1.dwHighDateTime;
  //ui2.LowPart  = ft2.dwLowDateTime;
  //ui2.HighPart = ft2.dwHighDateTime;
  //
  //this->m_WinDifference = 
  //  static_cast< TimeStampType >( ui2.QuadPart - ui1.QuadPart) / 
  //  static_cast< TimeStampType >( 1e7 );
  //
  //FILETIME currentTime;
  //LARGE_INTEGER intTime;
  //LARGE_INTEGER tick;
  //
  //::GetSystemTimeAsFileTime( &currentTime );
  //::QueryPerformanceCounter( &tick );
  //
  ////memcpy( &intTime, &currentTime, sizeof( intTime ) );
  //intTime.LowPart  = currentTime.dwLowDateTime;
  //intTime.HighPart = currentTime.dwHighDateTime;
  //
  //this->m_WinOrigin = 
  //  static_cast< TimeStampType >( intTime.QuadPart ) / 
  //  static_cast< TimeStampType >( 1e7 );
  //
  //this->m_WinOrigin -= 
  //  static_cast< TimeStampType >( (__int64)tick.QuadPart ) / 
  //  this->m_WinFrequency;
  //  
  //this->m_WinOrigin +=  this->m_WinDifference;
  //
  //this->m_Frequency = static_cast<igtlInt32>( m_WinFrequency );

  this->m_WinTimeOrigin = time( NULL );
  this->m_WinOrigin = clock();
  this->m_Frequency = 1000000; // fake


#else

  this->m_Frequency = 1000000;

#endif  // defined(WIN32) || defined(_WIN32)
}


TimeStamp::~TimeStamp()
{
}


void TimeStamp::GetTime()
{
#if defined(WIN32) || defined(_WIN32)

  //// The following code is deactiveted tempolorary due to unexpected behavior
  //// on 64-bit platform

  //LARGE_INTEGER tick;
  //
  //::QueryPerformanceCounter( &tick );
  //
  //TimeStampType value = 
  //    static_cast< TimeStampType >( (__int64)tick.QuadPart ) / 
  //    this->m_WinFrequency;
  //
  //value += this->m_WinOrigin;
  //
  //double second = floor(value);
  //this->m_Second = static_cast<igtlInt32>(second);
  //this->m_Nanosecond = static_cast<igtlInt32>((value - second)*1e9);

  clock_t c1 = clock();
  this->m_Second     = static_cast<igtlInt32>(this->m_WinTimeOrigin + ( c1 - this->m_WinClockOrigin ) / CLOCKS_PER_SEC);
  this->m_Nanosecond = static_cast<igtlInt32>((c1 - this->m_WinClockOrigin ) % CLOCKS_PER_SEC * ( 1e9 / CLOCKS_PER_SEC ));

#else

  struct timeval tval;

  ::gettimeofday( &tval, 0 );

  this->m_Second     = tval.tv_sec;
  this->m_Nanosecond = tval.tv_usec * 1000; /* convert from micro to nano */

#endif  // defined(WIN32) || defined(_WIN32)
  
}

void TimeStamp::SetTime(double tm)
{
  double second = floor(tm);
  this->m_Second = static_cast<igtlInt32>(second);
  this->m_Nanosecond = static_cast<igtlInt32>((tm - second)*1e9);
}

void TimeStamp::SetTime(igtlUint32 second, igtlUint32 nanosecond)
{
  if (nanosecond < 1e9)
    {
    this->m_Second = second;
    this->m_Nanosecond = nanosecond;
    }
}


void TimeStamp::SetTime(igtlUint64 tm)
{
  // Export from 64-bit fixed-point expression used in OpenIGTLink
  igtlInt32 sec      = static_cast<igtlInt32>((tm >> 32 ) & 0xFFFFFFFF);
  igtlInt32 fraction = static_cast<igtlInt32>(tm & 0xFFFFFFFF);
  this->m_Second     = sec;
  this->m_Nanosecond = igtl_frac_to_nanosec(static_cast<igtlUint32>(fraction));
}


double TimeStamp::GetTimeStamp()
{
  double tm;
  tm = static_cast<double>(this->m_Second) +
       static_cast<double>(this->m_Nanosecond) / 1e9;

  return tm;
}

void TimeStamp::GetTimeStamp(igtlUint32* second, igtlUint32* nanosecond)
{
  *second     = this->m_Second;
  *nanosecond = this->m_Nanosecond;
}
  
  
igtlUint64 TimeStamp::GetTimeStampUint64()
{
  // Export as 64-bit fixed-point expression used in OpenIGTLink
  igtlInt32 sec      = this->m_Second;
  igtlInt32 fraction = igtl_nanosec_to_frac(this->m_Nanosecond);

  igtlUint64 ts  =  sec & 0xFFFFFFFF;
  ts = (ts << 32) | (fraction & 0xFFFFFFFF);

  return ts;
}


void TimeStamp::PrintSelf( std::ostream& os) const
{
  Superclass::PrintSelf(os);

  std::string indent = "    ";

  os << indent << "Frequency of the clock: "
    << this->m_Frequency << std::endl;
  os << indent << "Second : "
    << this->m_Second << std::endl;
  os << indent << "Nanosecond : "
    << this->m_Nanosecond << std::endl;
}

}


