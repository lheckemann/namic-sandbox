################################
# Note:
# Shapes shall be the same size!
################################

PROJECT(ConvertImageToLevelSet)

SET(LIBS
  ITKCommon
  ITKNumerics
  ITKBasicFilters
  ITKIO
  itkvnl  
)

SET(SRCS
  toSFLS.cxx
)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS} )
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBS})
