#include <iostream>

// needed for unit testing
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "TestPoistats.h"

int TestPoistatsDriver(int argc, char* argv[]) {
  int isFailure = 0;
  
  RunnableTest *runnableTest = new TestPoistats();
  isFailure = !(runnableTest->RunTest());
  
  return isFailure;
}
