#ifndef TEST_POISTATS_REPLICAS_H_
#define TEST_POISTATS_REPLICAS_H_

#include <cppunit/extensions/HelperMacros.h>

#include "RunnableTest.h"
#include "../PoistatsReplicas.h"
#include "../PoistatsModel.h"

class TestPoistatsReplicas : public CppUnit::TestFixture, public RunnableTest
{
      
  // the test suite
  CPPUNIT_TEST_SUITE( TestPoistatsReplicas );

    CPPUNIT_TEST( TestRethreadPath );
  
    CPPUNIT_TEST( TestCopyCurrentToPreviousEnergy );

    CPPUNIT_TEST( TestSortArray );

    CPPUNIT_TEST( TestGetMinimumCurrentEnergy );
    
    CPPUNIT_TEST( TestGetCurrentMeanOfEnergies );

    CPPUNIT_TEST( TestGetNormalizedMeanCurrentPreviousEnergiesDifference );

    CPPUNIT_TEST( TestGetRandomSortedFirstSecondReplicas );

    CPPUNIT_TEST( TestCoolTemperatures );

    CPPUNIT_TEST( TestSetInitialPoints );

    CPPUNIT_TEST( TestGetBestTrialPaths );

  CPPUNIT_TEST_SUITE_END();
  
private:
  void Register();
  
  PoistatsModel *m_PoistatsModel;
  
  PoistatsReplicas *m_Replicas;

public:
  // these are required.  setUp is called before every test and tearDown after  
  void setUp();
  void tearDown();

  void TestRethreadPath();  
    
  void TestCopyCurrentToPreviousEnergy();

  void TestSortArray();
  
  void TestGetMinimumCurrentEnergy();
  
  void TestGetCurrentMeanOfEnergies();
  
  void TestGetNormalizedMeanCurrentPreviousEnergiesDifference();
  
  void TestGetRandomSortedFirstSecondReplicas();
  
  void TestCoolTemperatures();
  
  void TestSetInitialPoints();

  void TestGetBestTrialPaths();

};


#endif /*TEST_POISTATS_REPLICAS_H*/
