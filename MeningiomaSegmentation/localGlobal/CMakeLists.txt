project(MeningiomaSegmentationModule)

cmake_minimum_required(VERSION 2.6)

#
# Disable the warnings that DevStudio 2005 emits wrt to sprintf, strcpu, etc.
#
IF(CMAKE_COMPILER_2005)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
ENDIF(CMAKE_COMPILER_2005)


#
# Find Slicer3. This will locate all the components needed to build modules 
# for Slicer3.
#
FIND_PACKAGE(Slicer3 REQUIRED)
IF (Slicer3_FOUND)
  INCLUDE(${Slicer3_USE_FILE})    
ELSE (Slicer3_FOUND)
  MESSAGE(ERROR "Cannot build without a Slicer3 build tree or a Slicer3 installation. Please set Slicer3_DIR. If using a Slicer3 build, Slicer3_DIR needs to be set to the root of the build tree of Slicer3.  If using an installation of Slicer3, Slicer3_DIR needs to be set to <SlicerInstallation>/lib/Slicer3.")
ENDIF (Slicer3_FOUND)



# # Need to include class headers and the configuration header.
# INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
# INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

#########################################################
# OPTION(BUILD_SLICER_MODULE "Build Slicer CLI module."
#        ${VTK_BUILD_SHARED_LIBS} ON)

# IF(BUILD_SLICER_MODULE)
  
#   FIND_PACKAGE(Slicer3 REQUIRED)
#   INCLUDE(${Slicer3_USE_FILE})

#   GENERATECLP(MeningiomaSeg_SRCS MeningiomaSeg.xml )

#   ADD_EXECUTABLE(${PROJECT_NAME}
#     ${MeningiomaSeg_SRCS}
#     MeningiomaSegModule.cxx
#     )
    
#   TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${MeningiomaSeg_LIBS})

#   slicer3_set_plugins_output_path(${PROJECT_NAME})
#   if (Slicer3_SOURCE_DIR)
#     set(TARGETS ${PROJECT_NAME} ${PROJECT_NAME})
#     slicer3_install_plugins(${TARGETS})
#   endif (Slicer3_SOURCE_DIR)

#   #ADD_LIBRARY(${PROJECT_NAME}Module SHARED ${MeningiomaSeg_SRCS})
#   #slicer3_set_plugins_output_path(${PROJECT_NAME}Module)
#   #set_target_properties (${PROJECT_NAME}Module PROPERTIES
#   #  COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
#   #TARGET_LINK_LIBRARIES(${PROJECT_NAME}Module ${MeningiomaSeg_LIBS})  

#   if (Slicer3_SOURCE_DIR)
#     #set(TARGETS ${PROJECT_NAME}Module ${PROJECT_NAME})
#     #slicer3_install_plugins(${TARGETS})
#   endif (Slicer3_SOURCE_DIR)

# ENDIF(BUILD_SLICER_MODULE)


# FIND_PACKAGE(Slicer3 REQUIRED)
# INCLUDE(${Slicer3_USE_FILE})

#GENERATECLP(MeningiomaSeg_SRCS MeningiomaSeg.xml )
#
# Define our new plugin that uses ITK.  This plugin is compiled as both a 
# shared object and an executable
#

# SET ( ExampleModule_SOURCE ExampleModule.cxx )
# GENERATECLP(ExampleModule_SOURCE ExampleModule.xml)
# ADD_LIBRARY(ExampleModuleLib SHARED ${ExampleModule_SOURCE})
# SET_TARGET_PROPERTIES (ExampleModuleLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
# TARGET_LINK_LIBRARIES ( ExampleModuleLib ITKStatistics ITKIO ITKBasicFilters ITKCommon)
# ADD_EXECUTABLE ( ExampleModule ${ExampleModule_SOURCE})
# TARGET_LINK_LIBRARIES ( ExampleModule ITKStatistics ITKIO ITKBasicFilters ITKCommon)

SET (MeningiomaSeg_SRCS MeningiomaSegModule.cxx)
GENERATECLP(MeningiomaSeg_SRCS MeningiomaSeg.xml)
ADD_EXECUTABLE ( MeningiomaSegModule ${MeningiomaSeg_SRCS})
TARGET_LINK_LIBRARIES ( MeningiomaSegModule ITKCommon ITKBasicFilters ITKIO itkvnl)



# ADD_EXECUTABLE(${PROJECT_NAME}
#     ${MeningiomaSeg_SRCS}
#     MeningiomaSegModule.cxx
# )

#TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${MeningiomaSeg_LIBS})

#
# Install into a local plugins directory
#
set(TARGETS MeningiomaSegModule)
foreach(targ ${TARGETS})
  slicer3_set_plugins_output_path(${targ})
endforeach(targ ${TARGETS})
slicer3_install_plugins(${TARGETS})


# slicer3_set_plugins_output_path(${PROJECT_NAME})
# if (Slicer3_SOURCE_DIR)
#     set(TARGETS ${PROJECT_NAME} ${PROJECT_NAME})
#     slicer3_install_plugins(${TARGETS})
#     endif (Slicer3_SOURCE_DIR)

#   #ADD_LIBRARY(${PROJECT_NAME}Module SHARED ${MeningiomaSeg_SRCS})
#   #slicer3_set_plugins_output_path(${PROJECT_NAME}Module)
#   #set_target_properties (${PROJECT_NAME}Module PROPERTIES
#   #  COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
#   #TARGET_LINK_LIBRARIES(${PROJECT_NAME}Module ${MeningiomaSeg_LIBS})  

#     if (Slicer3_SOURCE_DIR)
#     #set(TARGETS ${PROJECT_NAME}Module ${PROJECT_NAME})
#     #slicer3_install_plugins(${TARGETS})
#         endif (Slicer3_SOURCE_DIR)







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
