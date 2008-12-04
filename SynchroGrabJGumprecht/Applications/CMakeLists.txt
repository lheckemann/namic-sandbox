INCLUDE_DIRECTORIES(${ULTERIUS_INCLUDE_DIR})

ADD_EXECUTABLE(SynchroGrab 
  vtkSynchrograbPipeline.cxx
#  vtkUltrasoundVolumeReconstructor.cxx #JG: Not in use yet
#  vtkUltrasoundVolumeSender.cxx #JG: Not in use yet
  SynchroGrab.cxx 
  )

IF(USE_TRACKER_DEVICE)
  #Use tracker device
  TARGET_LINK_LIBRARIES(
    SynchroGrab 
    OpenIGTLink 
    ##12/1/08
    ## NH commented out for compilation in Mac OS
    #ws2_32
    #wsock32
    vtkOpenIGTUltrasound 
    vtkIO
    ndicapi
    ${NDIOAPI_LIBRARY} 
  )
ELSE(USE_TRACKER_DEVICE)
  #Use tracker simulator
  TARGET_LINK_LIBRARIES(
    SynchroGrab 
    OpenIGTLink 
    ##12/1/08
    ## NH commented out for compilation in Mac OS
    #ws2_32
    #wsock32
    vtkOpenIGTUltrasound 
    vtkIO
)
ENDIF(USE_TRACKER_DEVICE)
