#include <cppunit/extensions/HelperMacros.h>
#include <boost/scoped_ptr.hpp>

#include <cli/CLI.h>

using namespace CLI;

class OptionTest : public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE( OptionTest );
  CPPUNIT_TEST( testOptionConstructors );
  CPPUNIT_TEST( testGetId );
  CPPUNIT_TEST( testGetOpt );
  CPPUNIT_TEST( testGetLongOpt );
  CPPUNIT_TEST( testArgNameMethods );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {
  }

  void tearDown() {
  }

  void testOptionConstructors() {
        Option option("w", "write-protect region");
        CPPUNIT_ASSERT(!option.hasArg());
        CPPUNIT_ASSERT(!option.hasLongOpt());
        CPPUNIT_ASSERT( option.getDescription() == std::string( "write-protect region" ) );
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

};

CPPUNIT_TEST_SUITE_REGISTRATION( OptionTest );
