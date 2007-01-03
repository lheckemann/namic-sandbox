#ifndef TEST_POISTATS_REPLICA_H_
#define TEST_POISTATS_REPLICA_H_

#include <cppunit/extensions/HelperMacros.h>

#include "RunnableTest.h"
#include "../PoistatsReplica.h"

class TestPoistatsReplica : public CppUnit::TestFixture, public RunnableTest
{
      
  // the test suite
  CPPUNIT_TEST_SUITE( TestPoistatsReplica );
  
    CPPUNIT_TEST( TestGenerateUnitSphereRandom );

    CPPUNIT_TEST( TestGenerateConstrainedRandomPoint3D );

    CPPUNIT_TEST( TestCopyPath );
    
    CPPUNIT_TEST( TestFoundBestPath );    
    
  CPPUNIT_TEST_SUITE_END();
  
private:
  void Register();
  
  PoistatsModel *m_PoistatsModel;
  
  PoistatsReplica *m_Replica;

public:
  // these are required.  setUp is called before every test and tearDown after  
  void setUp();
  void tearDown();
  
  void TestGenerateUnitSphereRandom();

  void TestGenerateConstrainedRandomPoint3D();

  void TestCopyPath();
  
  void TestFoundBestPath();
  
};


#endif /*TEST_POISTATS_REPLICA_H*/
