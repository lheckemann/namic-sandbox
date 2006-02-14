#include <cppunit/extensions/HelperMacros.h>

#include <cli/CLI.h>

using namespace CLI;

class OptionTest : public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE( OptionTest );
  CPPUNIT_TEST( testOptionConstructors );
  CPPUNIT_TEST( testCopyConstructor );
  CPPUNIT_TEST( testGetId );
  CPPUNIT_TEST( testGetOpt );
  CPPUNIT_TEST( testGetLongOpt );
  CPPUNIT_TEST( testArgNameMethods );
  CPPUNIT_TEST( testAddGetValue );
  CPPUNIT_TEST_EXCEPTION( testNoArgsException, CLI::ParseException );
  CPPUNIT_TEST_EXCEPTION( testTooManyArgsException, CLI::ParseException );
  CPPUNIT_TEST( testAddMultipleValues );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {
  }

  void tearDown() {
  }

  void testOptionConstructors() {
        Option option("w", "write protect specified region");
        CPPUNIT_ASSERT(!option.hasArg());
        CPPUNIT_ASSERT(!option.hasLongOpt());
        CPPUNIT_ASSERT( option.getDescription() == std::string( "write protect specified region" ) );
  }

  void testCopyConstructor() {
        Option option("w", "write-protect-region", true, "write protect specified region");
        CPPUNIT_ASSERT( option.hasArg() );
        CPPUNIT_ASSERT( option.hasLongOpt());
        CPPUNIT_ASSERT( option.getDescription() == std::string( "write protect specified region" ) );
        std::string value( "33" );
        option.addValue( value );
        CPPUNIT_ASSERT_EQUAL( value, option.getValue() );
        Option option2( option );
        CPPUNIT_ASSERT( option2.hasArg() );
        CPPUNIT_ASSERT( option2.hasLongOpt() );
        CPPUNIT_ASSERT( option2.getDescription() == std::string( "write protect specified region" ) );
        CPPUNIT_ASSERT_EQUAL( value, option2.getValue() );
  }

  void testGetId() {
        Option option( "w", "write-protect region" );
        CPPUNIT_ASSERT_EQUAL( (int)'w', option.getId() );
    }

  void testGetOpt() {
        Option option( "w", "force dibbot to wither on the vine" );
        CPPUNIT_ASSERT_EQUAL( std::string("w"), option.getOpt() );
        CPPUNIT_ASSERT( !option.hasLongOpt() );
    }

  void testGetLongOpt() {
        std::string longOpt( "wither-on-the-vine" );
        Option option( "w", longOpt.c_str(), true, "force dibbot to wither on the vine" );
        CPPUNIT_ASSERT( option.hasLongOpt() );
        CPPUNIT_ASSERT_EQUAL( longOpt, option.getLongOpt() );
    }

    void testArgNameMethods() {
        Option option( "h", "hemorrage-laser-factor", true, "hemorrage laser factor setting" );
        CPPUNIT_ASSERT( !option.hasArgName() );
        std::string argName( "factor" );
        option.setArgName( argName ); 
        CPPUNIT_ASSERT( option.hasArgName() );
        CPPUNIT_ASSERT_EQUAL( argName, option.getArgName() );
    }

    void testAddGetValue() {
        Option option;
        option.setArgs( Option::ONE_ARG );
        std::string value("this is your life, Franny Farmer");
        option.addValue(value);
        CPPUNIT_ASSERT_EQUAL( value, option.getValue() );
    }

    void testNoArgsException() {
        Option option;
        std::string value("this is your life, Franny Farmer");
        option.addValue(value);
        CPPUNIT_ASSERT_EQUAL( value, option.getValue() );
    }

    void testTooManyArgsException() {
        Option option;
        option.setArgs( Option::ONE_ARG );
        std::string value("this is your life, Franny Farmer");
        option.addValue(value);
        option.addValue(value);
        CPPUNIT_ASSERT_EQUAL( value, option.getValue() );
    }

    void testAddMultipleValues() {
        Option option;
        option.setArgs( Option::MANY_ARGS );
        std::string value("this is your life, Franny Farmer");
        option.addValue(value);
        CPPUNIT_ASSERT_EQUAL( value, option.getValue(0u) );
        std::string value2("this is your life, Smokey the Bear");
        option.addValue(value2);
        CPPUNIT_ASSERT_EQUAL( value2, option.getValue(1u) );
    }


};

CPPUNIT_TEST_SUITE_REGISTRATION( OptionTest );
