project(RobustStatisticsSegmentationModule)

cmake_minimum_required(VERSION 2.6)

#
# Disable the warnings that DevStudio 2005 emits wrt to sprintf, strcpu, etc.
#
if(CMAKE_COMPILER_2005)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
endif(CMAKE_COMPILER_2005)


#
# Find Slicer3. This will locate all the components needed to build modules 
# for Slicer3.
#
find_package(Slicer3 REQUIRED)
if (Slicer3_FOUND)
  include(${Slicer3_USE_FILE})    
else (Slicer3_FOUND)
  message(ERROR "Cannot build without a Slicer3 build tree or a Slicer3 installation. Please set Slicer3_DIR. If using a Slicer3 build, Slicer3_DIR needs to be set to the root of the build tree of Slicer3.  If using an installation of Slicer3, Slicer3_DIR needs to be set to <SlicerInstallation>/lib/Slicer3.")
endif (Slicer3_FOUND)




# ######################################################
# ##  OpenMP
# ######################################################
# INCLUDE(CheckFunctionExists)
# MESSAGE(STATUS "Check for compiler OpenMP support...")
# SET(OPENMP_FLAGS)
# SET(OPENMP_LIBRARIES)
# SET(OPENMP_FOUND FALSE)

# # Key: CFLAGS##LDFLAGS#LIBRARIES
# # Neither CFLAGS nor LDFLAGS can be empty.  Use NONE instead.
# SET(
#   OPENMP_FLAGS_AND_LIBRARIES
#   # gcc
#   "-fopenmp##-fopenmp#"
#   "-fopenmp##-fopenmp#gomp"
#   "-fopenmp##-fopenmp#gomp pthread"
#   # icc
#   "-openmp##-openmp#"
#   "-openmp -parallel##-openmp -parallel#"
#   # SGI & PGI
#   "-mp##-mp#"
#   # Sun
#   "-xopenmp##-xopenmp#"
#   # Tru64
#   "-omp##-omp#"
#   # AIX
#   "-qsmp=omp##-qsmp=omp#"
#   # MSVC
#   "/openmp##NONE#"
# )

# # Massive hack to workaround CMake limitations
# LIST(LENGTH OPENMP_FLAGS_AND_LIBRARIES NUM_FLAGS)
# MATH(EXPR NUM_FLAGS "${NUM_FLAGS} - 1")
# FOREACH(I RANGE 0 ${NUM_FLAGS})
#   IF(NOT OPENMP_FOUND)
#     LIST(GET OPENMP_FLAGS_AND_LIBRARIES ${I} TMP)
#     STRING(REGEX MATCH "([^#]*)" OPENMP_FLAGS ${TMP})
#     STRING(REGEX REPLACE "[^#]*##" "" TMP ${TMP})
#     STRING(REGEX MATCH "([^#]*)" OPENMP_LDFLAGS ${TMP})
#     STRING(REGEX REPLACE "[^#]*#" "" OPENMP_LIBRARIES ${TMP})
#     #MESSAGE(STATUS "OPENMP_FLAGS=${OPENMP_FLAGS}")
#     #MESSAGE(STATUS "OPENMP_LDFLAGS = ${OPENMP_LDFLAGS}")
#     #MESSAGE(STATUS "OPENMP_LIBRARIES = ${OPENMP_LIBRARIES}")
#     #MESSAGE(STATUS "-------")

#     IF(OPENMP_LDFLAGS MATCHES "NONE")
#       SET(OPENMP_LDFLAGS "")
#     ENDIF(OPENMP_LDFLAGS MATCHES "NONE")
#     IF(OPENMP_LIBRARIES MATCHES " ")
#       STRING(REPLACE " " ";" OPENMP_LIBRARIES ${OPENMP_LIBRARIES})
#     ENDIF(OPENMP_LIBRARIES MATCHES " ")

#     ## I think I need to do a try-compile
#     SET(CMAKE_REQUIRED_FLAGS ${OPENMP_FLAGS})
#     SET(CMAKE_REQUIRED_LIBRARIES ${OPENMP_LIBRARIES})
#     CHECK_FUNCTION_EXISTS(omp_get_thread_num OPENMP_FOUND${I})

#     IF(OPENMP_FOUND${I})
#       SET(OPENMP_FOUND TRUE)
#     ENDIF(OPENMP_FOUND${I})
#   ENDIF(NOT OPENMP_FOUND)
# ENDFOREACH(I RANGE 0 ${NUM_FLAGS})

# IF(OPENMP_FOUND)
#   MESSAGE(STATUS "OpenMP flags \"${OPENMP_FLAGS}\", OpenMP libraries \"${OPENMP_LIBRARIES}\"")
# ELSE(OPENMP_FOUND)
#   MESSAGE(STATUS "Given compiler does not support OpenMP.")
# ENDIF(OPENMP_FOUND)

# set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}\ ${OPENMP_FLAGS})
# set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS}\ ${OPENMP_FLAGS})









# # boost
# find_path(BOOST_PATH /boost/shared_ptr.hpp)
# if(NOT BOOST_PATH)
#    message(FATAL_ERROR "Please enter the path of boost")
# endif(NOT BOOST_PATH)

# include_directories(${BOOST_PATH})


# # find cArray path
# find_path(CARRAY_PATH cArray2D.h)
# if(NOT CARRAY_PATH)
#    message(FATAL_ERROR "Please enter the path of cArray.*")
# endif(NOT CARRAY_PATH)

# include_directories(${CARRAY_PATH})



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

set(seg_SRC SFLSRobustStat3DTest.cxx)
GENERATECLP(seg_SRC SFLSRobustStat3DTest.xml)
add_executable( SFLSRobustStat3DModule ${seg_SRC})
target_link_libraries( SFLSRobustStat3DModule ITKCommon ITKBasicFilters ITKIO itkvnl)



# ADD_EXECUTABLE(${PROJECT_NAME}
#     ${seg_SRC}
#     SFLSRobustStat3DModule.cxx
# )

#TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${MeningiomaSeg_LIBS})

#
# Install into a local plugins directory
#
set(TARGETS SFLSRobustStat3DModule)
foreach(targ ${TARGETS})
  slicer3_set_plugins_output_path(${targ})
endforeach(targ ${TARGETS})
slicer3_install_plugins(${TARGETS})


# slicer3_set_plugins_output_path(${PROJECT_NAME})
# if (Slicer3_SOURCE_DIR)
#     set(TARGETS ${PROJECT_NAME} ${PROJECT_NAME})
#     slicer3_install_plugins(${TARGETS})
#     endif (Slicer3_SOURCE_DIR)

#   #ADD_LIBRARY(${PROJECT_NAME}Module SHARED ${seg_SRC})
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
#     ${seg_SRC}
#     meningiomaSegmentor.cxx
#     )
#     TARGET_LINK_LIBRARIES(${PROJECT_NAME}Test ${MeningiomaSeg_LIBS})    
#   ENDIF(ITK_FOUND)

# ENDIF(BUILD_TEST)
