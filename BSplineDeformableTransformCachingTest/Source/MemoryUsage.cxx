
#include "MemoryUsage.h"
#include <sstream>

MemoryUsage::MemoryUsage() 
#ifdef __linux
: m_ParseSmaps()
#endif
{
  m_StartMemory = 0;
  m_StopMemory = 0;
  m_Starts = 0;
  m_Stops = 0;
}

// Memory usage in kilobytes.
MemoryUsage::SizeType 
MemoryUsage::GetMemoryUsage()
  {
#ifdef MEMORY_USE_KW_SYS
    this->m_SystemInformation.QueryMemory();
    // Careful about overflow.
    return 1024 * (this->m_SystemInformation.GetAvailableVirtualMemory() + 
                   this->m_SystemInformation.GetAvailablePhysicalMemory());
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
#elif __linux

    int pid = getpid();
    //std::cout << "Process id : " << pid << std::endl;

    std::stringstream filenameBuf;
    filenameBuf << "/proc/" << pid << "/smaps";

    std::string filename = filenameBuf.str();

    m_ParseSmaps.ParseFile( filename );

    return m_ParseSmaps.GetMemoryUsage("0:0");
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

void
MemoryUsage::Start()
{
  m_Starts++;
  m_StartMemory += GetMemoryUsage();
}

void 
MemoryUsage::Stop()
{
  m_Stops++;
  m_StopMemory += GetMemoryUsage();
}

double 
MemoryUsage::GetMeanMemoryChange() const
{
#ifdef MEMORY_USE_KW_SYS
  // KWSys gives available memory, not memory use.
  return (static_cast<double>(m_StartMemory) - static_cast<double>(m_StopMemory))/static_cast<double>(m_Starts);
#else
  return (static_cast<double>(m_StopMemory) - static_cast<double>(m_StartMemory))/static_cast<double>(m_Starts);
#endif
}

MemoryUsage::SizeType
MemoryUsage::GetNumberOfStarts() const
{
  return m_Starts;
}

MemoryUsage::SizeType
MemoryUsage::GetNumberOfStops() const
{
  return m_Stops;
}

