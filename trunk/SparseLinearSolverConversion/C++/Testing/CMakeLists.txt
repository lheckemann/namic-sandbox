SET(KIT lsqr)
CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
  lsqrTest1.cxx
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CxxTests.cxx)

ADD_EXECUTABLE(${KIT}CxxTests ${Tests})
TARGET_LINK_LIBRARIES(${KIT}CxxTests lsqr)

SET( KWScene_Core_TESTS ${CXX_TEST_PATH}/${KIT}CxxTests)

