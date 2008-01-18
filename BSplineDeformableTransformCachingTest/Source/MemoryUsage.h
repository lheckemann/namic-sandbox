
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
  SizeType GetMemoryUsage()
  {
#ifdef MEMORY_USE_KW_SYS
    this->m_SystemInformation.QueryMemory();
    return this->m_SystemInformation.GetAvailableVirtualMemory() * 1024;
#else
#ifdef WIN32
    DWORD pid = GetCurrentProcessId();
    PROCESS_MEMORY_COUNTERS memoryCounters;

    HANDLE  hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
                                     PROCESS_VM_READ,
                                     FALSE, pid );

    if (NULL == hProcess)
      {
      // Can't determine memory usage.
      return 0;
      }
  
    GetProcessMemoryInfo( hProcess, &memoryCounters, sizeof(memoryCounters));

    SizeType mem = static_cast<SizeType>( 
                            static_cast<double>( memoryCounters.PagefileUsage )
                                                                            / 1024.0 );

    return mem;
#else
    // Maybe use getrusage() ??
    rusage resourceInfo;

    int who = RUSAGE_SELF;
    if (getrusage(who, &resourceInfo) == 0)
      {
      return resourceInfo.ru_ixrss;
      }

    return 0;
#endif
#endif
  }

  MemoryUsage()
    {
    m_StartMemory = 0;
    m_StopMemory = 0;
    m_Starts = 0;
    m_Stops = 0;
    }

  void Start()
    {
    m_Starts++;
    m_StartMemory += GetMemoryUsage();
    }

  void Stop()
    {
    m_Stops++;
    m_StopMemory += GetMemoryUsage();
    }

  double GetMeanMemoryChange() const
    {
#ifdef MEMORY_USE_KW_SYS
    // KWSys gives available memory, not memory use.
    return (static_cast<double>(m_StartMemory) - static_cast<double>(m_StopMemory))/static_cast<double>(m_Starts);
#else
    return (static_cast<double>(m_StopMemory) - static_cast<double>(m_StartMemory))/static_cast<double>(m_Starts);
#endif
    }

  SizeType GetNumberOfStarts() const
    {
    return m_Starts;
    }

  SizeType GetNumberOfStops() const
    {
    return m_Stops;
    }

protected:

  SizeType m_StartMemory;
  SizeType m_StopMemory;
  SizeType m_Starts;
  SizeType m_Stops;

#ifdef MEMORY_USE_KW_SYS
  itksys::SystemInformation m_SystemInformation;
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


