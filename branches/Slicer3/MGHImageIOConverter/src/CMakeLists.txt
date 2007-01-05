PROJECT(mgh_test)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
  "Cannot build without ITK. Please set ITK_DIR.")
ENDIF(ITK_FOUND)

IF(EXISTS "${ITK_DIR}/CMakeCache.txt")
  INCLUDE_DIRECTORIES("${ITK_DIR}/Utilities/zlib")
ELSE(EXISTS "${ITK_DIR}/CMakeCache.txt")
ENDIF(EXISTS "${ITK_DIR}/CMakeCache.txt")

ADD_EXECUTABLE(mgh_test test_read_data.cxx itkMGHImageIO.cxx itkMGHImageIOFactory.cxx)

TARGET_LINK_LIBRARIES(mgh_test ITKIO)
