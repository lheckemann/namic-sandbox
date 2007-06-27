/**
  *  James:  Write a program that will test the itkAnalyzeObjectEntry class.  This program needs to return 0 if successfull and -1 if failure.
  */
#include "objectentry.h"

int main (int argc, char * argv[])
{
  int error_count = 0 ;
  itk::AnalyzeObjectEntry::Pointer testAOE01=itk::AnalyzeObjectEntry::New();
  testAOE01->SetDisplayFlag(true);
  testAOE01->SetOpacity(0.123);

  itk::AnalyzeObjectEntry::Pointer testAOE02=itk::AnalyzeObjectEntry::New();
  testAOE02->Copy(testAOE01); //Stress the operator=
  if( testAOE02->GetOpacity() != 0.123 )
    {
    error_count++;
    }
  return error_count;
}

