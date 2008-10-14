INCLUDE_DIRECTORIES(${ULTERIUS_INCLUDE_DIR})

ADD_EXECUTABLE(SynchroGrab SynchroGrab.cxx vtkSynchroGrabPipeline.cxx)
TARGET_LINK_LIBRARIES(SynchroGrab 
OpenIGTLink 
ws2_32
wsock32
vtkOpenIGTUltrasound 
vtkIO 
ndicapi
${NDIOAPI_LIBRARY} 
)
