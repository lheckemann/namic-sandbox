#include "objectmap.h"

int main (int argc, char * argv[])
{
    int error_count = 0 ;
    
  itk::AnalyzeObjectEntry::Pointer testAOE01=itk::AnalyzeObjectEntry::New();

  itk::AnalyzeObjectMap::Pointer testAOE02 = itk::AnalyzeObjectMap::New();

  //const bool objectReadStatus = testAOE02->ReadObjectFile(argv[1]);
  const bool objectReadStatus = testAOE02->ReadObjectFile("C:/Documents and Settings/woofton/Desktop/object_good/Data/test.obj");
  const bool objectWriteStatus = testAOE02->WriteObjectFile("C:/Documents and Settings/woofton/Desktop/object_good/Data/testWrite6.obj");

  std::ifstream ReferenceFile;
  ReferenceFile.open("C:/Documents and Settings/woofton/Desktop/object_good/Data/test.obj", std::ios::binary | std::ios::in);

  std::ifstream WrittenFile;
  WrittenFile.open("C:/Documents and Settings/woofton/Desktop/object_good/Data/testWrite6.obj", std::ios::binary | std::ios::in);

  char ReferenceBytes;
  char WrittenBytes;
  int count = 0;
  std::ofstream myfile;
  myfile.open("testing1.txt");
  while(!ReferenceFile.eof()  && !WrittenFile.eof())
  {
      count++;
      ReferenceFile.read(reinterpret_cast<char *> (&ReferenceBytes), sizeof(char));
      WrittenFile.read(reinterpret_cast<char *>(&WrittenBytes), sizeof(char));
      if(ReferenceBytes != WrittenBytes)
      {
          error_count++;
          myfile<<count<< ":" << (int)ReferenceBytes << ":" << (int)WrittenBytes << std::endl;
      }
  }

  if(!ReferenceFile.eof())
  {
      error_count++;
      std::cout<<"ReferenceFile is not at end of file"<<std::endl;
  }
  if(!WrittenFile.eof())
  {
      error_count++;
      std::cout<<"WrittenFile is not at end of file"<<std::endl;
  }

  ReferenceFile.close();
  WrittenFile.close();
  if(!objectReadStatus)
  {
      error_count++;
  }

  if(!objectWriteStatus)
  {
      error_count++;
  }


  if( error_count )
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
