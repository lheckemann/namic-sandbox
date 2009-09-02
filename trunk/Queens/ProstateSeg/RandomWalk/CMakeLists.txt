CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

PROJECT(ProstateSeg)

SET(ProstateSeg_SRCS
  cVolume.cxx
  cVolOp.cxx
  spdEqSolver.cxx
  randomWalkSeg.cxx
)

# On Windows add also header files to the project
# (required for object browsing in the Microsoft Visual C++ IDE) 
IF (WIN32)
  IF (NOT CYGWIN)
    SET(ProstateSeg_SRCS ${ProstateSeg_SRCS}
      cVolOp.h
      cVolume.h
      randomWalkSeg.h
      spdEqSolver.h 
  )
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

SET(ProstateSeg_LIBS
  ITKCommon
  ITKBasicFilters
  ITKIO
  itkvnl
)

# Need to include class headers and the configuration header.
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

#########################################################
OPTION(BUILD_SLICER_MODULE "Build Slicer CLI module."
       ${VTK_BUILD_SHARED_LIBS} OFF)

IF(BUILD_SLICER_MODULE)
  
  FIND_PACKAGE(Slicer3 REQUIRED)
  INCLUDE(${Slicer3_USE_FILE})

  GENERATECLP(ProstateSeg_SRCS ProstateSeg.xml )

  ADD_EXECUTABLE(${PROJECT_NAME}
    ${ProstateSeg_SRCS}
    ProstateSegModule.cxx
    )
    
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${ProstateSeg_LIBS})

  slicer3_set_plugins_output_path(${PROJECT_NAME})
  if (Slicer3_SOURCE_DIR)
    set(TARGETS ${PROJECT_NAME} ${PROJECT_NAME})
    slicer3_install_plugins(${TARGETS})
  endif (Slicer3_SOURCE_DIR)

  #ADD_LIBRARY(${PROJECT_NAME}Module SHARED ${ProstateSeg_SRCS})
  #slicer3_set_plugins_output_path(${PROJECT_NAME}Module)
  #set_target_properties (${PROJECT_NAME}Module PROPERTIES
  #  COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
  #TARGET_LINK_LIBRARIES(${PROJECT_NAME}Module ${ProstateSeg_LIBS})  

  if (Slicer3_SOURCE_DIR)
    #set(TARGETS ${PROJECT_NAME}Module ${PROJECT_NAME})
    #slicer3_install_plugins(${TARGETS})
  endif (Slicer3_SOURCE_DIR)

ENDIF(BUILD_SLICER_MODULE)

#########################################################
OPTION(BUILD_TEST "Build test executable."
       ${VTK_BUILD_TEST} ON)

IF(BUILD_TEST)

  FIND_PACKAGE (ITK)
  IF (ITK_FOUND)
     INCLUDE(${USE_ITK_FILE})
  ENDIF(ITK_FOUND) 

  IF(ITK_FOUND)
    ADD_EXECUTABLE(${PROJECT_NAME}Test 
    ${ProstateSeg_SRCS}
    randomWalkSegTest.cxx
    )
    TARGET_LINK_LIBRARIES(${PROJECT_NAME}Test ${ProstateSeg_LIBS})    
  ENDIF(ITK_FOUND)

ENDIF(BUILD_TEST)
