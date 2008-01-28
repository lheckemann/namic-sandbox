
#ifndef __MEMORY_USAGE_H_
#define __MEMORY_USAGE_H_

#include <map>
#include <string>
#include <iostream>

//#define MEMORY_USE_KW_SYS

#ifdef MEMORY_USE_KW_SYS
#include <itksys/SystemInformation.hxx>
#endif 

#ifdef WIN32
  #include <windows.h>
  #include <psapi.h>
#elif __linux
#include "ParseSmaps.h"
#else
  #include <sys/time.h>
  #include <sys/resource.h>
//  #include <unistd.h>
#endif


class MemoryUsage
{
public:
  typedef unsigned long SizeType;

  // Memory usage in kilobytes.
  SizeType GetMemoryUsage();

  MemoryUsage();

  void Start();

  void Stop();

  double GetMeanMemoryChange() const;

  SizeType GetNumberOfStarts() const;

  SizeType GetNumberOfStops() const;

protected:

  SizeType m_StartMemory;
  SizeType m_StopMemory;
  SizeType m_Starts;
  SizeType m_Stops;

#ifdef MEMORY_USE_KW_SYS
  itksys::SystemInformation m_SystemInformation;
#endif

#ifdef __linux
  ParseSmaps m_ParseSmaps;
#endif

};

class MemoryUseCollector
{
public:
  typedef MemoryUsage       ProbeType;
  typedef std::string                IdType;
  typedef std::map<IdType,ProbeType> MapType;

  virtual void Start( const char * name )
    {
    IdType tid = name;
    MapType::iterator  pos = m_Probes.find( tid );
    if ( pos == m_Probes.end() )
      {
      m_Probes[ tid ] = ProbeType();
      }
    m_Probes[ tid ].Start(); 
    }

  virtual void Stop( const char * name )
    {
    IdType tid = name;
    MapType::iterator  pos = m_Probes.find( tid );
    if ( pos == m_Probes.end() )
      {
      return;
      }
    pos->second.Stop(); 
    }
  
  virtual void Report( std::ostream & os = std::cout ) const
    {
    MapType::const_iterator probe = m_Probes.begin();
    MapType::const_iterator end   = m_Probes.end();

    os << "-----------------Start Memory Report----------------------" << std::endl;
    os.width(25);
    os <<  " Probe Tag ";
    os.width(10);
    os <<  " Starts ";
    os.width(10);
    os <<  " Stops  ";
    os.width(15);
    os <<  "  KB Change  ";
    os << std::endl;
    os << std::endl;

    while( probe != end )
      {
      os.width(25);
      os <<  probe->first.c_str() << "  ";
      os.width(10);
      os <<  probe->second.GetNumberOfStarts() <<  "   ";
      os.width(10);
      os <<  probe->second.GetNumberOfStops() <<  "   ";
      os.width(15);
      os <<  probe->second.GetMeanMemoryChange();
      os << std::endl;
      probe++;
      }
    os << "-------------------End Memory Report----------------------" << std::endl;

    }

protected:

  MapType   m_Probes;
};

#endif //  __MEMORY_USAGE_H_


