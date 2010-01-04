SET(KIT SbiaHardiSlicerIntegration)
CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
  vtkMRMLDiffusionRSHDisplayPropertiesNodeTest1.cxx
  EXTRA_INCLUDE slicerTestingMacros.h
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CxxTests.cxx)

SET(LIBRARY_NAME ${PROJECT_NAME})

# Add path to find slicerTestingMacros.h
INCLUDE_DIRECTORIES(
  ${SBIA_HARDI_SOURCE_DIR}/Testing
  )

ADD_EXECUTABLE(${KIT}CxxTests ${Tests})
TARGET_LINK_LIBRARIES(${KIT}CxxTests ${lib_name})

SET( SbiaHardiSlicerIntegrationLib_TESTS ${CXX_TEST_PATH}/${KIT}CxxTests)

IF(WIN32)
  SET(SbiaHardiSlicerIntegrationLib_TESTS ${CXX_TEST_PATH}/${CMAKE_BUILD_TYPE}/${KIT}CxxTests)
ENDIF(WIN32)

MACRO( SIMPLE_TEST  TESTNAME )
  ADD_TEST( ${TESTNAME} ${LAUNCH_EXE} ${SbiaHardiSlicerIntegrationLib_TESTS} ${TESTNAME} )
ENDMACRO( SIMPLE_TEST  )


SIMPLE_TEST( vtkMRMLDiffusionRSHDisplayPropertiesNodeTest1 )

#ADD_EXECUTABLE(SphereHarmonicTest SphereHarmonicTest.cxx)
#TARGET_LINK_LIBRARIES( SphereHarmonicTest vtkRendering ${lib_name} )

ADD_EXECUTABLE(GlyphTest GlyphTest.cxx )
TARGET_LINK_LIBRARIES( GlyphTest vtkRendering ${lib_name} ITKIO ${VTK_LIBRARIES} )

