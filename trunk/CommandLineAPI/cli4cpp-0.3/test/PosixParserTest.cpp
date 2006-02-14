#include <cppunit/extensions/HelperMacros.h>

#include <cli/ParseException.h>
#include <cli/PosixParser.h>

using namespace CLI;

class PosixParserTest : public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE( PosixParserTest );
  CPPUNIT_TEST( testGetArgs );
  CPPUNIT_TEST( testCompressedArgs );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {
  }

  void tearDown() {
  }

  void testGetArgs() {
        const char* argv[] = {
            "test", "one", "two", "three", 0
        };
        const int argc(4);
        CLI::Options options;
        options.addOption("b", "burbles-per-milgot", true, "Specify the number of burbles per milgot");
        options.addOption("c", "cannibalize", false, "Run in  \"cannibalize\" mode");
        CLI::PosixParser parser;
        CLI::CommandLine cl( parser.parse(options, argc, argv) );
        std::vector<std::string> args = cl.getArgs();
        CPPUNIT_ASSERT(args.size() == 3);
        CPPUNIT_ASSERT_EQUAL(std::string("one"), args[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("two"), args[1]);
        CPPUNIT_ASSERT_EQUAL(std::string("three"), args[2]);
    }

  void testCompressedArgs() {
        const char* argv[] = {
            "test", "--burbles-per-milgot", "19", "-xyz", "one", "two", "three", 0
        };
        const int argc(sizeof(argv) / sizeof(argv[0]) - 1);
        CLI::Options options;
        options.addOption("b", "burbles-per-milgot", true, "Specify the number of burbles per milgot");
        options.addOption("c", "cannibalize", false, "Run in  \"cannibalize\" mode");
        options.addOption( CLI::Option("x", "hex format") );
        options.addOption( CLI::Option( "y", "yizzle-frizzizle format") );
        options.addOption( CLI::Option( "z", "zed multiplier") );
        CLI::PosixParser parser;
        CLI::CommandLine cl( parser.parse(options, argc, argv) );
        std::vector<std::string> args = cl.getArgs();
        CPPUNIT_ASSERT(args.size() == 3);
        CPPUNIT_ASSERT_EQUAL(std::string("one"), args[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("two"), args[1]);
        CPPUNIT_ASSERT_EQUAL(std::string("three"), args[2]);
        CPPUNIT_ASSERT( cl.hasOption('x') );
        CPPUNIT_ASSERT( cl.hasOption('y') );
        CPPUNIT_ASSERT( cl.hasOption('z') );
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( PosixParserTest );
