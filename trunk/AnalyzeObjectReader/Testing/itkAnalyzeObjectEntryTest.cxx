/**
  *  James:  Write a program that will test the itkAnalyzeObjectEntry class.  This program needs to return 0 if successfull and -1 if failure.
  */
#include "objectentry.h"

int main (int argc, char * argv[])
{
  itk::AnalyzeObjectEntry::Pointer testAOE=itk::AnalyzeObjectEntry::New();

  testAOE->SetDisplayFlag(true);

  return 0;
}
