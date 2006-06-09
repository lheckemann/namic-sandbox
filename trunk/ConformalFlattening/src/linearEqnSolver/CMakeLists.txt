SET(f "linearEqnSolverTest")
SET(a "linearEqnSolverTest.out")

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

PROJECT(${f})
INCLUDE_REGULAR_EXPRESSION("^.*$")

FIND_PACKAGE (ITK)
IF (ITK_FOUND)
   INCLUDE(${USE_ITK_FILE})
ENDIF(ITK_FOUND) 

ADD_LIBRARY(linearEqnSolver linearEqnSolver.cxx)
 
ADD_EXECUTABLE(${a} ${f}.cxx)
TARGET_LINK_LIBRARIES(${a} linearEqnSolver ITKCommon ITKIO itkvnl)# itkvnl_algo)

