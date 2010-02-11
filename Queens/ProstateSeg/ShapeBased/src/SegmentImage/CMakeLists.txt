PROJECT(ProstateSegSB)

SET(ProstateSegSB_SRCS
  shapeBasedGAC.cxx
  shapeBasedSeg.cxx
  shapeBasedSegCost.cxx
)

SET(ProstateSegSB_LIBS
  ITKCommon
  ITKNumerics
  ITKBasicFilters
  ITKIO
  itkvnl
)

# Need to include class headers and the configuration header.
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

#########################################################
# Build Slicer module

IF(BUILD_SLICER_MODULE)
  
  FIND_PACKAGE(Slicer3 REQUIRED)
  INCLUDE(${Slicer3_USE_FILE})

  GENERATECLP(ProstateSegSB_SRCS ProstateSegSB.xml )

  ADD_EXECUTABLE(${PROJECT_NAME}
    ${ProstateSegSB_SRCS}
    ProstateSegSBModule.cxx
    )
    
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${ProstateSegSB_LIBS})

  slicer3_set_plugins_output_path(${PROJECT_NAME})
  if (Slicer3_SOURCE_DIR)
    set(TARGETS ${PROJECT_NAME} ${PROJECT_NAME})
    slicer3_install_plugins(${TARGETS})
  endif (Slicer3_SOURCE_DIR)

  #ADD_LIBRARY(${PROJECT_NAME}Module SHARED ${ProstateSegSB_SRCS})
  #slicer3_set_plugins_output_path(${PROJECT_NAME}Module)
  #set_target_properties (${PROJECT_NAME}Module PROPERTIES
  #  COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
  #TARGET_LINK_LIBRARIES(${PROJECT_NAME}Module ${ProstateSegSB_LIBS})  

  if (Slicer3_SOURCE_DIR)
    #set(TARGETS ${PROJECT_NAME}Module ${PROJECT_NAME})
    #slicer3_install_plugins(${TARGETS})
  endif (Slicer3_SOURCE_DIR)

ENDIF(BUILD_SLICER_MODULE)

#########################################################
# Build test executable

ADD_EXECUTABLE(${PROJECT_NAME}Test 
  ${ProstateSegSB_SRCS}
  wholeSeg.cxx
)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}Test ${ProstateSegSB_LIBS})    

