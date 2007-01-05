#include <cppunit/extensions/HelperMacros.h>

#include <cli/ParseException.h>
#include <cli/BasicParser.h>

using namespace CLI;

class BasicParserTest : public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE( BasicParserTest );
  CPPUNIT_TEST_EXCEPTION( testArgumentMissing, CLI::ParseException );
  CPPUNIT_TEST( testGetArgs );
  CPPUNIT_TEST( testGetArgsAndOptions );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {
  }

  void tearDown() {
  }

    /*
     * no args
     * no options - testGetArgs
     * option missing argument
     * option with argument
     * repeated option with argument
     */

  void testArgumentMissing() {
        const char* argv[] = {
            "test", "-b", 0
        };
        const int argc(2);
        CLI::Options options;
        options.addOption("b", "burbles-per-milgot", true, "Specify the number of burbles per milgot");
        CLI::BasicParser parser;
        CLI::CommandLine cl( parser.parse(options, argc, argv) );
    }

  void testGetArgs() {
        const char* argv[] = {
            "test", "one", "two", "three", 0
        };
        const int argc(4);
        CLI::Options options;
        options.addOption("b", "burbles-per-milgot", true, "Specify the number of burbles per milgot");
        options.addOption("c", "cannibalize", false, "Run in  \"cannibalize\" mode");
        CLI::BasicParser parser;
        CLI::CommandLine cl( parser.parse(options, argc, argv) );
        std::vector<std::string> args = cl.getArgs();
        CPPUNIT_ASSERT(args.size() == 3);
        CPPUNIT_ASSERT_EQUAL(std::string("one"), args[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("two"), args[1]);
        CPPUNIT_ASSERT_EQUAL(std::string("three"), args[2]);
    }

  void testGetArgsAndOptions() {
        const char* argv[] = {
            "test", "--burbles-per-milgot", "19", "one", "two", "three", 0
        };
        const int argc(sizeof(argv) / sizeof(argv[0]) - 1);
        CLI::Options options;
        options.addOption("b", "burbles-per-milgot", true, "Specify the number of burbles per milgot");
        options.addOption("c", "cannibalize", false, "Run in  \"cannibalize\" mode");
        CLI::BasicParser parser;
        CLI::CommandLine cl( parser.parse(options, argc, argv) );
        std::vector<std::string> args = cl.getArgs();
        CPPUNIT_ASSERT(args.size() == 3);
        CPPUNIT_ASSERT_EQUAL(std::string("one"), args[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("two"), args[1]);
        CPPUNIT_ASSERT_EQUAL(std::string("three"), args[2]);
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( BasicParserTest );
