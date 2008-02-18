
#include "MachineInformation.h"
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[])
{
  if (argc < 2 )
    {
    MachineInformation::DescribeMachine( std::cout );
    }
  else
    {
    std::string filename = argv[1];
    std::ofstream output( filename.c_str() );
    MachineInformation::DescribeMachineLatex( output );
    }

  return 0;
}
