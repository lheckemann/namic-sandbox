#ifndef TEST_POISTATS_H_
#define TEST_POISTATS_H_

#include <cppunit/extensions/HelperMacros.h>

#include "../TensorReaderStrategy.h"
#include "../itkPoistatsFilter.h"

#include "RunnableTest.h"

class TestPoistats : public CppUnit::TestFixture, public RunnableTest
{
      
  // the test suite
  CPPUNIT_TEST_SUITE( TestPoistats );
  
    CPPUNIT_TEST( TestSetSeedVolume );

    CPPUNIT_TEST( TestGenerateRotationMatrix3u2v );
    
    CPPUNIT_TEST( TestGetMagnetToSliceFrameRotation );

    CPPUNIT_TEST( TestCalculateTensor2Odf );

// TODO: this takes too long to run all the time...    
    CPPUNIT_TEST( TestCalculateOdfPathEnergy );

    CPPUNIT_TEST( TestCalculateDensityMatrix );
        
//    CPPUNIT_TEST( TestCalculate );
    
    CPPUNIT_TEST( TestRoundPath );
    
// commented out the source for the moment.  It's kind of tested in ContrucOdfList
// TODO: this takes too long to run all the time...    
//    CPPUNIT_TEST( TestGetOdfsAtPoints );

// TODO: takes too long to run
//    CPPUNIT_TEST( TestConstructOdfList );

    CPPUNIT_TEST( TestGetDistance );
    
    CPPUNIT_TEST( TestGetPointClosestToCenter );

    CPPUNIT_TEST( TestGetCenterOfMass );

    CPPUNIT_TEST( TestConvertPointsToImage );
    
    CPPUNIT_TEST( TestGetPositiveMinimumInt );

    CPPUNIT_TEST( TestGetPositiveMaximumInt );
        
// TODO: I'm suspicious of the results of the probabilities    
//    CPPUNIT_TEST( TestGetPathProbabilities );

// TODO: takes too long to run
//    CPPUNIT_TEST( TestGetAggregateReplicaDensities );
    
//    CPPUNIT_TEST( TestMask );    
  
  CPPUNIT_TEST_SUITE_END();
  
private:
  void Register();

public:
  // these are required.  setUp is called before every test and tearDown after  
  void setUp();
  void tearDown();
    
  void TestRethreadPath();

  void TestGenerateUnitSphereRandom();

  void TestGenerateRotationMatrix3u2v();
  
  void TestCalculateTensor2Odf();
  
  void TestCalculateOdfPathEnergy();
    
  void TestCalculate();
  
  void TestRoundPath();
  
  void TestGetOdfsAtPoints();
  
  void TestConstructOdfList();
  
  void TestGetDistance();
  
  void TestGetPointClosestToCenter();
  
  void TestGetCenterOfMass();
  
  void TestGetInitialPoints();
  
  void TestConvertPointsToImage();
  
  void TestGetPositiveMinimumInt();

  void TestGetPositiveMaximumInt();
  
  void TestSetSeedVolume();
  
  void TestGetPathProbabilities();
  
  void TestGetAggregateReplicaDensities();
  
  void TestMask();
  
  void TestGetMagnetToSliceFrameRotation();

  void TestCalculateDensityMatrix();
  
  static void GetOdfs( itk::Array2D< double >* odfs );

  static void GetOdfLookUpTable( itk::Array< double >* table );
  
};


#endif /*TEST_POISTATS_H_*/
