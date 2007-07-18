#include "objectmap.h"

int main (int argc, char * argv[])
{
    int error_count = 0 ;
    
  itk::AnalyzeObjectEntry::Pointer testAOE01=itk::AnalyzeObjectEntry::New();

  itk::AnalyzeObjectMap::Pointer testAOE02 = itk::AnalyzeObjectMap::New();

  //const bool objectReadStatus = testAOE02->ReadObjectFile(argv[1]);
  const bool objectReadStatus = testAOE02->ReadObjectFile("C:/Documents and Settings/woofton/Desktop/object_good/Data/test.obj");
  const bool objectWriteStatus = testAOE02->WriteObjectFile("C:/Documents and Settings/woofton/Desktop/object_good/Data/testWrite6.obj");

  std::ifstream test;
  test.open("C:/Documents and Settings/woofton/Desktop/object_good/Data/test.obj", std::ios::binary | std::ios::in);

  std::ifstream testWrite;
  testWrite.open("C:/Documents and Settings/woofton/Desktop/object_good/Data/testWrite6.obj", std::ios::binary | std::ios::in);

  char testing;
  char testing2;
  int count = 0;
  std::ofstream myfile;
  myfile.open("testing.txt");
  while(!test.eof()  && !testWrite.eof())
  {
      count++;
      test.read(&testing, sizeof(char));
      testWrite.read(&testing2, sizeof(char));
      if(testing != testing2)
      {
          error_count++;
          //std::cout<<"Testing does not equal testing2"<<std::endl;
          myfile<<count<<std::endl;
          myfile<<testing<<std::endl;
          myfile<<testing2<<std::endl;
      }
  }

  if(!test.eof())
  {
      error_count++;
      std::cout<<"Test is not at end of file"<<std::endl;
  }
  if(!testWrite.eof())
  {
      error_count++;
      std::cout<<"testWrite is not at end of file"<<std::endl;
  }

  test.close();
  testWrite.close();
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
