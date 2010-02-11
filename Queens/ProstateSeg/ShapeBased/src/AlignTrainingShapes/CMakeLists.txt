PROJECT(AlignTrainingShapes)

SET(SRCS
  ICPTest3.cxx
  kdtree2.cpp
  pointSetFileIO.cxx
  imageToPointSet.cxx
  ICP3.cxx
  ICPCost3.cxx
  imgRegByPtSet3D.cxx
  imgRegByPtSet3D_uchar.cxx
)
#imgRegByPtSet3DTest.cxx
#imgRegByPtSet3DTest_uchar.cxx 

SET(LIBS
  ITKCommon
  ITKNumerics
  ITKBasicFilters
  ITKIO
)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS} )
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBS})
