#include <iostream>

#include <cppunit/extensions/HelperMacros.h>

#include <cli/CLI.h>

using namespace CLI;

class OptionsTest : public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE( OptionsTest );
  CPPUNIT_TEST( testAddOption );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {
  }

  void tearDown() {
  }

  void testAddOption() {
        CLI::Options options;
        options.addOption("b", "burbles-per-milgot", true, "Specify the number of burbles per milgot");
        CPPUNIT_ASSERT(options.hasOption("b"));
        CLI::Option option( options.getOption("b") );
        CPPUNIT_ASSERT_EQUAL(option.getOpt(), std::string("b"));
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION( OptionsTest );
