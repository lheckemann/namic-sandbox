
#include "MemoryUsage.h"
#include <stdlib.h>

int main(int argc, char* argv[])
{
  MemoryUseCollector memUse;

  memUse.Start("1KB Test");
  char* foo = new char[1*1024*1024];
  memUse.Stop("1KB Test");

  memUse.Start("70KB Test");
  char* foo2 = new char[70*1024*1024];
  memUse.Stop("70KB Test");

  memUse.Report( std::cout );
  return EXIT_SUCCESS;
}

