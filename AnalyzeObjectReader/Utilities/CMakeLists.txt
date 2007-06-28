PROJECT(AnalyzeObjectLibrary)


SET( AnalyzeObjectLibrary_SRCS
  objectentry.cxx
  objectmap.cxx
  )

ADD_LIBRARY( AnalyzeObjectIO ${AnalyzeObjectLibrary_SRCS} )

