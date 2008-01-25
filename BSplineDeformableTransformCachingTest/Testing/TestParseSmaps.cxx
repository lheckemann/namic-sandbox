
#include <stdlib.h>
#include <sstream>
#include <iostream>

#include "ParseSmaps.h"

int main( int argc, char* argv[] )
{
  ParseSmaps psmaps;
  int pid = getpid();
  std::cout << "Process id : " << pid << std::endl;

  std::stringstream filenameBuf;
  filenameBuf << "/proc/" << pid << "/smaps";

  std::string filename = filenameBuf.str();

  psmaps.ParseFile( filename );
  int heap0b = psmaps.GetMemoryUsage("0:0");
//  std::cout << "Before : " << heap0b << std::endl;
  char* alloc0 = new char[5*1024*1024];
  psmaps.ParseFile( filename );
//  std::cin >> alloc0[0];
  alloc0[0] = 3;
  int heap0a = psmaps.GetMemoryUsage("0:0");
//  std::cout << "After : " << heap0a << std::endl;

  std::cout << "Allocated : " << heap0a - heap0b << " kB." << std::endl;

  psmaps.ParseFile( filename );
  int heap1b = psmaps.GetMemoryUsage("0:0");
//  std::cout << "Before : " << heap1b << std::endl;
  char* alloc1 = new char[2*1024*1024];
  psmaps.ParseFile( filename );
//  std::cin >> alloc1[0];
  alloc1[0] = 3;
  int heap1a = psmaps.GetMemoryUsage("0:0");
//  std::cout << "After : " << heap1a << std::endl;

  std::cout << "Allocated : " << heap1a - heap1b << " kB." << std::endl;

  psmaps.ParseFile( filename );
  int heap2b = psmaps.GetMemoryUsage("0:0");
//  std::cout << "Before : " << heap2b << std::endl;
  char* alloc2 = new char[150*1024];
  psmaps.ParseFile( filename );
//  std::cin >> alloc2[0];
  alloc2[0] = 3;
  int heap2a = psmaps.GetMemoryUsage("0:0");
//  std::cout << "After : " << heap2a << std::endl;

  std::cout << "Allocated : " << heap2a - heap2b << " kB." << std::endl;
}
