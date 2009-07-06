# The header files
SET(HEADERS
  itkSingleBitBinaryImage.h
  itkSliceContiguousImage.h
  itkSparseImage.h
)

# The implementation files
SET(SOURCES
  itkSingleBitBinaryImage.txx
  itkSliceContiguousImage.txx
  itkSparseImage.txx
  perf01.cxx
)

# Add this as include directory
INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}
  ${SOURCE_PATH}
)

# Main library
ADD_EXECUTABLE(Perf01 ${HEADERS} ${SOURCES})
TARGET_LINK_LIBRARIES(Perf01 ${ITK_LIBRARIES})
