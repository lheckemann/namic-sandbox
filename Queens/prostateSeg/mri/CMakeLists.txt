SET(projName "prostateSegmentation")
PROJECT(${projName})
# INCLUDE_REGULAR_EXPRESSION("^.*$")

################################
# include ITK, to avoid setting it everytime, do:
# set the environment variable ITK_DIR to the dir containing ITKConfig.cmake in .bashrc
FIND_PACKAGE (ITK)
IF (ITK_FOUND)
   INCLUDE(${USE_ITK_FILE})
ENDIF(ITK_FOUND) 


ADD_LIBRARY(cVolume ../cVolume.cxx)
ADD_LIBRARY(cVolOp ../cVolOp.cxx)
ADD_LIBRARY(spdEqSolver ../spdEqSolver.cxx)
ADD_LIBRARY(randomWalkSeg ../randomWalkSeg.cxx)


SET(Libraries
randomWalkSeg
spdEqSolver
cVolOp
cVolume
ITKCommon
ITKBasicFilters
ITKIO
itkvnl
)


ADD_EXECUTABLE(randomWalkSegTest randomWalkSegTest.cxx)
TARGET_LINK_LIBRARIES(randomWalkSegTest ${Libraries})
