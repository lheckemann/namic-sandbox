#include <iostream>

// needed for unit testing
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "TestPoistatsReplicas.h"

int TestPoistatsReplicasDriver(int argc, char* argv[]) {
  int isFailure = 0;
  
  RunnableTest *runnableTest = new TestPoistatsReplicas();
  isFailure = !(runnableTest->RunTest());
  
  return isFailure;
}
