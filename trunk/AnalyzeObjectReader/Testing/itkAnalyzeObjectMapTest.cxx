#include "objectmap.h"

int main (int argc, char * argv[])
{
    int error_count = 0 ;
  itk::AnalyzeObjectEntry::Pointer testAOE01=itk::AnalyzeObjectEntry::New();

  itk::AnalyzeObjectMap::Pointer testAOE02 = itk::AnalyzeObjectMap::New();

  testAOE02->ReadObjectFile("D:/Nirep1/data/na0/na01.obj");

  //mary = testAOE02->ReadObjectFile("bob");

/*  if(mary)
  {
      error_count;
  }*/


  if( error_count )
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
