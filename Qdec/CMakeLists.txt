PROJECT(Qdec)

# build shared lib so can link from Slicer
ADD_DEFINITIONS(-DBUILD_SHARED_LIBS)

SET(CMAKE_BACKWARDS_COMPATABILITY 1.4)
# include a test for endianness (used to be done by default)
INCLUDE (${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(CMAKE_WORDS_BIGENDIAN)

# 
# Disable the warnings that DevStudio 2005 emits wrt sprintf, strcpu, etc.
#
IF(CMAKE_COMPILER_2005)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
ENDIF(CMAKE_COMPILER_2005)
  
CONFIGURE_FILE(
  ${Qdec_SOURCE_DIR}/vtkQdecConfigure.h.in
  ${Qdec_BINARY_DIR}/vtkQdecConfigure.h
)

INCLUDE_DIRECTORIES(${Qdec_SOURCE_DIR} ${Qdec_BINARY_DIR})

SET(Qdec_SRCS
QdecContrast.cpp 
QdecDataTable.cpp 
QdecFactor.cpp 
QdecGlmDesign.cpp 
QdecGlmFit.cpp 
QdecGlmFitResults.cpp 
QdecProject.cpp 
QdecSubject.cpp 
QdecUtilities.cpp
)

ADD_EXECUTABLE(qdecCmd QdecCmd.cpp ${Qdec_SRCS})

ADD_LIBRARY(Qdec SHARED ${Qdec_SRCS})

