

NeuroLib_USE_ITK() 
NeuroLib_USE_SHAPE()

ADD_EXECUTABLE(  SegPostProcess SegPostProcess.cxx ccl.c)

TARGET_LINK_LIBRARIES ( SegPostProcess ${ITK_LIBRARIES} )
