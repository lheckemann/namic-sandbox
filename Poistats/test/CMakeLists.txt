
INCLUDE_DIRECTORIES(
  /usr/pubsw/packages/cppunit/current/include
)

SET(DRIVERS_SRCS
  TestPoistatsDriver.cpp
  TestPoistatsReplicasDriver.cpp
  TestPoistatsReplicaDriver.cpp
)

SET(TEST_SRCS
  ../PoistatsModel.cxx
  ../PoistatsReplica.cxx
  ../PoistatsReplicas.cxx
  ../itkPoistatsFilter.txx
  TestPoistats.cpp
  TestPoistatsReplicas.cpp
  TestPoistatsReplica.cpp  
  RunnableTest.cpp
)

CREATE_TEST_SOURCELIST(TestDriver TestDriver.cpp
  ${DRIVERS_SRCS}
)

LINK_DIRECTORIES(
  /usr/pubsw/packages/cppunit/current/lib/
)

ADD_EXECUTABLE(TestDriver ${DRIVERS_SRCS} ${TEST_SRCS} TestDriver.cpp)

TARGET_LINK_LIBRARIES(TestDriver
  ITKCommon
  ITKBasicFilters
  ITKIO
  cppunit
)

# enable "make test"
#ENABLE_TESTING()
FOREACH (test ${DRIVERS_SRCS})
  GET_FILENAME_COMPONENT(TestName ${test} NAME_WE)
  ADD_TEST(${TestName} TestDriver ${TestName})
ENDFOREACH (test)
