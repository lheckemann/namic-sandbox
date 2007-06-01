PROJECT(Qdec)

SET(Qdec_SRCS
QdecCmd.cpp 
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

ADD_EXECUTABLE(qdecCmd ${Qdec_SRCS})



