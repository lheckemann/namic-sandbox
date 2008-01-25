
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "ParseSmaps.h"

int main( int argc, char* argv[] )
{
  if ( 1 == argc )
    {
    std::cerr << "Usage : input_smap_file" << std::endl;
    exit(EXIT_FAILURE);
    }


  ParseSmaps psmaps;
  std::string filename( argv[1] );
  psmaps.ParseFile( filename );
  std::cout << "Heap usage : " << psmaps.GetHeapUsage() << " kB" << std::endl;
  std::cout << "Stack usage  : " << psmaps.GetStackUsage() << " kB" << std::endl;
  std::cout << "Our process(?)  : " << psmaps.GetMemoryUsage("0:0") << " kB" << std::endl;

  psmaps.PrintMemoryUsage();

}
