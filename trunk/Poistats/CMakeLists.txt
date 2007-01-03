PROJECT (Poistats)

# put the ITK location here...
#########################################
#ITK location
#INCLUDE (${CMAKE_ROOT}/Modules/FindITK.cmake)
########################################

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

# put the ITK include directory here
#INCLUDE_DIRECTORIES(
#  ${ITK_INCLUDE_DIRS}
#)

SET(SRCS
  FreeDiffusionExecutable.cpp
  CommandParser.cpp
  poistats.cpp
  itkPoistatsFilter.txx
  PoistatsReplicas.cxx
  PoistatsReplica.cxx
  PoistatsModel.cxx 
  itkBSplineKernelFunction.txx
  itkBSplineScatteredDataPointSetToImageFilter.txx
  itkPointSetToImageFilter.txx
)

ADD_EXECUTABLE( poistats ${SRCS} )

# link the ITK library here
LINK_DIRECTORIES(
  ${ITK_LIBRARY_DIRS}
)

#freetype and X libs are needed for static qt library
TARGET_LINK_LIBRARIES( poistats
  ITKCommon
  ITKBasicFilters
  ITKAlgorithms 
  ITKIO
)

# enable "make test"
ENABLE_TESTING()

SUBDIRS(test)
