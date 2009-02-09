INCLUDE_DIRECTORIES(${ULTERIUS_INCLUDE_DIR})

ADD_EXECUTABLE(4DUltrasound
  4DUltrasound.cxx 
  vtkDataCollector.cxx 
  vtkDataProcessor.cxx
  vtkDataSender.cxx
  vtkInstrumentTracker.cxx
  )

IF(USE_TRACKER_DEVICE)
  #Use tracker device
  TARGET_LINK_LIBRARIES(
    4DUltrasound 
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
    4DUltrasound 
    OpenIGTLink 
    ##12/1/08
    ## NH commented out for compilation in Mac OS
    #ws2_32
    #wsock32
    vtkOpenIGTUltrasound 
    vtkIO
)
ENDIF(USE_TRACKER_DEVICE)
