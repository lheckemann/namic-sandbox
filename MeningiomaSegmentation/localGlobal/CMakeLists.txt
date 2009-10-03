CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

PROJECT(MeningiomaSeg)

SET(MeningiomaSeg_SRCS
)

# On Windows add also header files to the project
# (required for object browsing in the Microsoft Visual C++ IDE) 
IF (WIN32)
  IF (NOT CYGWIN)
    SET(MeningiomaSeg_SRCS ${MeningiomaSeg_SRCS}
#       cVolOp.h
#       cVolume.h
#       randomWalkSeg.h
#       spdEqSolver.h 
  )
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

SET(MeningiomaSeg_LIBS
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
       ${VTK_BUILD_SHARED_LIBS} ON)

IF(BUILD_SLICER_MODULE)
  
  FIND_PACKAGE(Slicer3 REQUIRED)
  INCLUDE(${Slicer3_USE_FILE})

  GENERATECLP(MeningiomaSeg_SRCS MeningiomaSeg.xml )

  ADD_EXECUTABLE(${PROJECT_NAME}
    ${MeningiomaSeg_SRCS}
    MeningiomaSegModule.cxx
    )
    
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${MeningiomaSeg_LIBS})

  slicer3_set_plugins_output_path(${PROJECT_NAME})
  if (Slicer3_SOURCE_DIR)
    set(TARGETS ${PROJECT_NAME} ${PROJECT_NAME})
    slicer3_install_plugins(${TARGETS})
  endif (Slicer3_SOURCE_DIR)

  #ADD_LIBRARY(${PROJECT_NAME}Module SHARED ${MeningiomaSeg_SRCS})
  #slicer3_set_plugins_output_path(${PROJECT_NAME}Module)
  #set_target_properties (${PROJECT_NAME}Module PROPERTIES
  #  COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
  #TARGET_LINK_LIBRARIES(${PROJECT_NAME}Module ${MeningiomaSeg_LIBS})  

  if (Slicer3_SOURCE_DIR)
    #set(TARGETS ${PROJECT_NAME}Module ${PROJECT_NAME})
    #slicer3_install_plugins(${TARGETS})
  endif (Slicer3_SOURCE_DIR)

ENDIF(BUILD_SLICER_MODULE)







# #########################################################
# OPTION(BUILD_TEST "Build local test executable."
#        ${VTK_BUILD_TEST} ON)

# IF(BUILD_TEST)

#   FIND_PACKAGE (ITK)
#   IF (ITK_FOUND)
#      INCLUDE(${USE_ITK_FILE})
#   ENDIF(ITK_FOUND) 

#   IF(ITK_FOUND)
#     ADD_EXECUTABLE(${PROJECT_NAME}Test 
#     ${MeningiomaSeg_SRCS}
#     meningiomaSegmentor.cxx
#     )
#     TARGET_LINK_LIBRARIES(${PROJECT_NAME}Test ${MeningiomaSeg_LIBS})    
#   ENDIF(ITK_FOUND)

# ENDIF(BUILD_TEST)
