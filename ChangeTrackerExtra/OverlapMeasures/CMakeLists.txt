cmake_minimum_required(VERSION 2.4)

PROJECT( LabelOverlapMeasures )

#The following lines are required to use Dart
ENABLE_TESTING()
INCLUDE(Dart)

# Set up ITK
IF(USE_ITK)
  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
      "Cannot build without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)
ENDIF(USE_ITK)

INCLUDE (${CMAKE_ROOT}/Modules/FindITK.cmake) 
IF (USE_ITK_FILE) 
  INCLUDE(${USE_ITK_FILE}) 
ENDIF(USE_ITK_FILE) 

LINK_DIRECTORIES(${ITK_LIBRARY_PATH})

IF(BUILD_TESTING)
# non-templated class -- this should be stored in a library and linked in...
ADD_EXECUTABLE(itkLabelOverlapMeasuresImageFilterTest itkLabelOverlapMeasuresImageFilterTest.cxx)
TARGET_LINK_LIBRARIES(itkLabelOverlapMeasuresImageFilterTest ITKIO)

ENDIF(BUILD_TESTING)

###
#  Perform testing
###

ADD_TEST( TEST1 ${CMAKE_BINARY_DIR}/itkLabelOverlapMeasuresImageFilterTest 2
          ${CMAKE_SOURCE_DIR}/sourceImage.nii.gz
          ${CMAKE_SOURCE_DIR}/targetImage.nii.gz )
