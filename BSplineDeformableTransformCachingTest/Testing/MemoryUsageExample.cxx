
#include "MemoryUsage.h"
#include <stdlib.h>

int main(int argc, char* argv[])
{
  MemoryUseCollector memUse;

  memUse.Start("1KB Test");
  char* foo = new char[1*1024];
  memUse.Stop("1KB Test");

  memUse.Start("70KB Test");
  char* foo2 = new char[70*1024];
  memUse.Stop("70KB Test");

  memUse.Start("170KB Test");
  char* foo3 = new char[170*1024];
  memUse.Stop("170KB Test");

  memUse.Start("1024KB Test");
  char* foo4 = new char[1024*1024];
  memUse.Stop("1024KB Test");


  memUse.Report( std::cout );
  return EXIT_SUCCESS;
}

