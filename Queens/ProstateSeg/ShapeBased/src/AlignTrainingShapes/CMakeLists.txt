PROJECT(AlignTrainingShapes)

SET(SRCS
  imgRegByPtSet3DTest_uchar.cxx 
#  ICPTest3.cxx
  kdtree2.cpp
  pointSetFileIO.cxx
  imageToPointSet.cxx
  ICP3.cxx
  ICPCost3.cxx
  imgRegByPtSet3D.cxx
  imgRegByPtSet3D_uchar.cxx
)
#imgRegByPtSet3DTest.cxx

INCLUDE_DIRECTORIES(../shared/arrays)

SET(LIBS
  ITKCommon
  ITKNumerics
  ITKBasicFilters
  ITKIO
)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS} )
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBS})
