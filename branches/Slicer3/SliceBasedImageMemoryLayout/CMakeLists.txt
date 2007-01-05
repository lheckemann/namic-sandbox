# This project is designed to be built outside the Insight source tree.
PROJECT(ImageMemoryLayout)

# Find ITK.
FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

INCLUDE_DIRECTORIES(
  ${ImageMemoryLayout_SOURCE_DIR}/Source
  ${ImageMemoryLayout_SOURCE_DIR}/Testing
  )

SUBDIRS(
  Source 
  Testing
  )

