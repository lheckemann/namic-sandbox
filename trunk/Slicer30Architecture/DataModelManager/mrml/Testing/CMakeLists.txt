# Define the tests
SET(TESTS_SRCS
  testInstance
  TestAllMrml
  )

# Add the include paths
INCLUDE_DIRECTORIES(
  ${MRML_SOURCE_DIR}
  )

CREATE_TEST_SOURCELIST(Tests mrmlTests.cxx ${TESTS_SRCS})
ADD_EXECUTABLE(mrmlTests ${Tests})
TARGET_LINK_LIBRARIES(mrmlTests mrml)

#Don't understand why I need that ??
SET(MRML_TESTS "${EXECUTABLE_OUTPUT_PATH}/mrmlTests")

# Loop over files and create executables
FOREACH(name ${TEST_SRCS})
  ADD_TEST(${name} ${MRML_TESTS} ${name})
ENDFOREACH(name)


