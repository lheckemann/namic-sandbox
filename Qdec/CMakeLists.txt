PROJECT(Qdec)

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

INCLUDE_DIRECTORIES(${Qdec_SOURCE_DIR})

ADD_EXECUTABLE(qdecCmd QdecCmd.cpp ${Qdec_SRCS})

ADD_LIBRARY(Qdec ${Qdec_SRCS})

