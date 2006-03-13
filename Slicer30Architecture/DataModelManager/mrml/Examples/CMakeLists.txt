# Define the Examples
SET(EXAMPLES_SRCS
  Simple.cxx
  )

# Add the include paths
INCLUDE_DIRECTORIES(
  ${MRML_SOURCE_DIR}
  )

ADD_EXECUTABLE(Simple Simple.cxx)
TARGET_LINK_LIBRARIES(Simple vtkmrml)


