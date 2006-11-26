# This project is designed to be built outside the Insight source tree.
PROJECT(ImageMemoryLayout)

# Find ITK.
FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

SUBDIRS(
  Source 
  Testing
  )

