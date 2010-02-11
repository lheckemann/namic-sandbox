PROJECT(LearnShapeModel)

SET(SRCS
imagePCAAnalysisTest2.cxx 
)

SET(LIBS
  ITKCommon
  ITKNumerics
  ITKBasicFilters
  ITKIO
  itkvnl
)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS} )
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBS})
