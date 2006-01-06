#include <string>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/ui/text/TestRunner.h>

int main(int argc, char* argv[]) {
  std::string testPath = (argc > 1) ? std::string(argv[1]) : "";

  // Get the top level suite from the registry
  CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );      

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;

  runner.addTest( suite );

  // Change the default outputter to a compiler error format outputter
  // runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(), std::cerr ) );
  // Run the tests.
    runner.run( controller, testPath );

  // Return error code 1 if the one of test failed.
    return result.testFailuresTotal() == 0 ? 0 : 1;
}
