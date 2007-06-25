PROJECT(AnalyzeObjectITKIO)

SET(AnalyzeObjectITKIO_SRCS
  itkAnalyzeObjectImageIO.cxx
  )

ADD_LIBRARY(AnalyzeObjectITKIO  ${AnalyzeObjectITKIO_SRCS})

TARGET_LINK_LIBRARIES(AnalyzeObjectITKIO  AnalyzeObjectIO)

