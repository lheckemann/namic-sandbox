
#include "MachineInformation.h"

// from kwsys
#include <itksys/SystemInformation.hxx>


void
MachineInformation::DescribeMachine( std::ostream& os )
{
  itksys::SystemInformation systemInformation;

  systemInformation.RunCPUCheck();
  systemInformation.RunOSCheck();
  systemInformation.RunMemoryCheck();
  
  os << "Vendor string :               " << systemInformation.GetVendorString() << std::endl;
  os << "Vendor ID :                   " << systemInformation.GetVendorID() << std::endl;
  os << "Processor name :              " << systemInformation.GetExtendedProcessorName() << std::endl;
  os << "Type ID :                     " << systemInformation.GetTypeID() << std::endl;
  os << "Family ID :                   " << systemInformation.GetFamilyID() << std::endl;
  os << "Model ID :                    " << systemInformation.GetModelID() << std::endl;
  os << "Stepping code :               " << systemInformation.GetSteppingCode() << std::endl;
  os << "Processor cache size :        " << systemInformation.GetProcessorCacheSize() << std::endl;
  os << "Processor clock frequency :   " << systemInformation.GetProcessorClockFrequency() << std::endl;
  os << "OS :                          " << systemInformation.GetOSName() << std::endl;
  os << "Hostname :                    " << systemInformation.GetHostname() << std::endl;
  os << "OS Release :                  " << systemInformation.GetOSRelease() << std::endl;
  //os << "OS Version :                  " << systemInformation.GetOSVersion() << std::endl;
  os << "OS Platform :                 " << systemInformation.GetOSPlatform() << std::endl;
  os << "64 bit :                      " << ( systemInformation.Is64Bits() ? "Yes" : "No") << std::endl;
  os << "Logical CPUs / physical :     " << systemInformation.GetNumberOfLogicalCPU() << std::endl;
  os << "Physical CPUs :               " << systemInformation.GetNumberOfPhysicalCPU() << std::endl;
  os << "Total Virtual Memory :        " << systemInformation.GetTotalVirtualMemory() << std::endl;
  os << "Avaiable Virtual Memory :     " << systemInformation.GetAvailableVirtualMemory() << std::endl;
  os << "Total Physical Memory :       " << systemInformation.GetTotalPhysicalMemory() << std::endl;
  os << "Available Physical Memory :   " << systemInformation.GetAvailablePhysicalMemory() << std::endl;

}

void
MachineInformation::DescribeMachineLatex( std::ostream& os )
{
  itksys::SystemInformation systemInformation;

  systemInformation.RunCPUCheck();
  systemInformation.RunOSCheck();
  systemInformation.RunMemoryCheck();
  
  os << "\\begin{center}" << std::endl;
  os << "\\begin{tabular}{ | l | c | }" << std::endl;
  os << "\\hline" << std::endl;
  os << "\\multicolumn{2}{|c|}{Test System Information} \\\\ \\hline" << std::endl;
  os << "Vendor string &               " << systemInformation.GetVendorString() << "\\\\ \\hline" << std::endl;
  os << "Vendor ID &                   " << systemInformation.GetVendorID() << "\\\\ \\hline" << std::endl;
  os << "Processor name &              " << systemInformation.GetExtendedProcessorName() << "\\\\ \\hline" << std::endl;
  os << "Type ID &                     " << systemInformation.GetTypeID() << "\\\\ \\hline" << std::endl;
  os << "Family ID &                   " << systemInformation.GetFamilyID() << "\\\\ \\hline" << std::endl;
  os << "Model ID &                    " << systemInformation.GetModelID() << "\\\\ \\hline" << std::endl;
  os << "Stepping code &               " << systemInformation.GetSteppingCode() << "\\\\ \\hline" << std::endl;
  os << "Processor cache size &        " << systemInformation.GetProcessorCacheSize() << "\\\\ \\hline" << std::endl;
  os << "Processor clock frequency (MHz) &   " << systemInformation.GetProcessorClockFrequency() << "\\\\ \\hline" << std::endl;
  os << "OS &                          " << systemInformation.GetOSName() << "\\\\ \\hline" << std::endl;
  os << "Hostname &                    " << systemInformation.GetHostname() << "\\\\ \\hline" << std::endl;
  os << "OS Release &                  " << systemInformation.GetOSRelease() << "\\\\ \\hline" << std::endl;
  //os << "OS Version &                  " << systemInformation.GetOSVersion() << "\\\\ \\hline" << std::endl;
  os << "OS Platform &                 " << systemInformation.GetOSPlatform() << "\\\\ \\hline" << std::endl;
  os << "64 bit &                      " << ( systemInformation.Is64Bits() ? "Yes" : "No") << "\\\\ \\hline" << std::endl;
  os << "Logical CPUs / physical &     " << systemInformation.GetNumberOfLogicalCPU() << "\\\\ \\hline" << std::endl;
  os << "Physical CPUs &               " << systemInformation.GetNumberOfPhysicalCPU() << "\\\\ \\hline" << std::endl;
  os << "Total Virtual Memory (MB)&        " << systemInformation.GetTotalVirtualMemory() << "\\\\ \\hline" << std::endl;
  os << "Avaiable Virtual Memory (MB)&     " << systemInformation.GetAvailableVirtualMemory() << "\\\\ \\hline" << std::endl;
  os << "Total Physical Memory (MB)&       " << systemInformation.GetTotalPhysicalMemory() << "\\\\ \\hline" << std::endl;
  os << "Available Physical Memory (MB)&   " << systemInformation.GetAvailablePhysicalMemory() << "\\\\ \\hline" << std::endl;
  os << "\\end{tabular}" << std::endl;
  os << "\\end{center}" << std::endl;

}
