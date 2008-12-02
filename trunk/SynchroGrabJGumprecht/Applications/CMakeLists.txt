INCLUDE_DIRECTORIES(${ULTERIUS_INCLUDE_DIR})


#ADD_EXECUTABLE(OpenIGTTest.cxx)


ADD_EXECUTABLE(SynchroGrab SynchroGrab.cxx vtkSynchroGrabPipeline.cxx)
TARGET_LINK_LIBRARIES(
SynchroGrab 
OpenIGTLink 
##12/1/08
## NH commented out for compilation in Mac OS
#ws2_32
#wsock32
vtkOpenIGTUltrasound 
vtkIO 
#ndicapi
#${NDIOAPI_LIBRARY} 
)
