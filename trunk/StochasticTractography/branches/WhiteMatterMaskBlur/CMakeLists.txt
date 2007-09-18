PROJECT(WhiteMatterMaskBlur)

SET(CMAKE_VERBOSE_MAKEFILE ON)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR,"ITK not found.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

#INCLUDE(/projects/schiz/software/Slicer3/Libs/GenerateCLP/UseGenerateCLP.cmake)
#INCLUDE(/home/tringo/Meng/Code/subversion/GenerateCLP/UseGenerateCLP.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/../../../GenerateCLP/UseGenerateCLP.cmake)
#INCLUDE(/afs/csail.mit.edu/u/t/tringo/subversion/Slicer3/Libs/GenerateCLP/UseGenerateCLP.cmake)

SET(WhiteMatterMaskBlur_SOURCE WhiteMatterMaskBlur.cxx)
GENERATECLP(WhiteMatterMaskBlur_SOURCE WhiteMatterMaskBlur.xml)
ADD_EXECUTABLE(WhiteMatterMaskBlur ${WhiteMatterMaskBlur_SOURCE})
TARGET_LINK_LIBRARIES(WhiteMatterMaskBlur ITKIO ITKBasicFilters ITKCommon)

SET(WhiteMatterMaskBlur_SOURCE WhiteMatterMaskBlur)
GENERATECLP(WhiteMatterMaskBlur WhiteMatterMaskBlur.xml)
ADD_EXECUTABLE(WhiteMatterMaskBlur ${WhiteMatterMaskBlur_SOURCE})
TARGET_LINK_LIBRARIES(WhiteMatterMaskBlur ITKIO ITKBasicFilters ITKCommon)
