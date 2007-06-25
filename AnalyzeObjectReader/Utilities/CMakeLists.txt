PROJECT(AnalyzeObjectLibrary)


SET( AnalyzeObjectLibrary_SRCS
  objectentry.cpp
  objectmap.cpp
  objectutils.cpp
  )

ADD_LIBRARY( AnalyzeObjectIO ${AnalyzeObjectLibrary_SRCS} )

