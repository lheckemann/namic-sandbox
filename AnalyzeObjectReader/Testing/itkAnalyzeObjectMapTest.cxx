#include "objectmap.h"

int main (int argc, char * argv[])
{
    int error_count = 0 ;
    
  itk::AnalyzeObjectEntry::Pointer testAOE01=itk::AnalyzeObjectEntry::New();

  itk::AnalyzeObjectMap::Pointer testAOE02 = itk::AnalyzeObjectMap::New();

  //const bool objectReadStatus = testAOE02->ReadObjectFile(argv[1]);
  const bool objectReadStatus = testAOE02->ReadObjectFile("D:/object_good/Data/test.obj");

  if(!objectReadStatus)
  {
      error_count++;
  }


  if( error_count )
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
