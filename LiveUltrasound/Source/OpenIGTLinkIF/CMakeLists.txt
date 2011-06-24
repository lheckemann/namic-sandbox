PROJECT(OpenIGTLinkIF)

#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.2)
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# See http://cmake.org/cmake/help/cmake-2-8-docs.html#section_Policies for details
#-----------------------------------------------------------------------------
if(POLICY CMP0016)
  CMAKE_POLICY(SET CMP0016 NEW)
endif()
if(POLICY CMP0017)
  CMAKE_POLICY(SET CMP0017 OLD)
endif()

# --------------------------------------------------------------------------
# Check if version 2 protocol is available
# Features that depnd on OpenIGTLink Protocol 2 are only available 
# if the OpenIGTLink library is compiled with OpenIGTLink_PROTOCOL_VERSION_2
# option.

if (OpenIGTLink_PROTOCOL_VERSION EQUAL 2)
  ADD_DEFINITIONS(-DOpenIGTLinkIF_USE_VERSION_2)
  ADD_DEFINITIONS(-DOpenIGTLinkIF_LIBRARY_VERSION="${OpenIGTLink_VERSION_MAJOR}-${OpenIGTLink_VERSION_MINOR}-${OpenIGTLink_VERSION_PATCH} for protocol ver. 2")
else (OpenIGTLink_PROTOCOL_VERSION EQUAL 2)
  ADD_DEFINITIONS(-DOpenIGTLinkIF_LIBRARY_VERSION="${OpenIGTLink_VERSION_MAJOR}-${OpenIGTLink_VERSION_MINOR}-${OpenIGTLink_VERSION_PATCH} for protocol ver. 1")
endif (OpenIGTLink_PROTOCOL_VERSION EQUAL 2) 


# --------------------------------------------------------------------------
# Add subdirectories

ADD_SUBDIRECTORY(Logic)


# --------------------------------------------------------------------------
# Include dirs

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Slicer_Libs_INCLUDE_DIRS}
  ${Slicer_Base_INCLUDE_DIRS}
  ${OpenIGTLink_INCLUDE_DIRS}
  ${OpenIGTLinkIFModuleLogic_SOURCE_DIR}
  ${OpenIGTLinkIFModuleLogic_BINARY_DIR}
  )

#
# Configure include file
#
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkOpenIGTLinkIFConfigure.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkOpenIGTLinkIFConfigure.h
  )

file(GLOB headers 
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  )

install(FILES 
  ${headers} 
  "${CMAKE_CURRENT_BINARY_DIR}/vtkOpenIGTLinkIFConfigure.h"
  DESTINATION ${Slicer_INSTALL_MODULES_INCLUDE_DIR}/${PROJECT_NAME} COMPONENT Development
  )


# --------------------------------------------------------------------------
# Sources

set(OpenIGTLinkIF_SRCS 
  vtkOpenIGTLinkIFGUI.cxx
  vtkIGTLTestWindow.cxx
)

if (OpenIGTLink_PROTOCOL_VERSION EQUAL 2)
  set(OpenIGTLinkIF_SRCS 
    ${OpenIGTLinkIF_SRCS}
    vtkIGTLRemoteDataListWindow.cxx
    vtkIGTLTrackingDataControllerWindow.cxx
    )
endif (OpenIGTLink_PROTOCOL_VERSION EQUAL 2)

# --------------------------------------------------------------------------
# Wrapping

include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
vtk_wrap_tcl3(OpenIGTLinkIF OpenIGTLinkIF_TCL_SRCS "${OpenIGTLinkIF_SRCS}" "")



#---------------------------------------------------------------------------
# Add Loadable Module support

generatelm(OpenIGTLinkIF_SRCS OpenIGTLinkIF.txt)

# --------------------------------------------------------------------------
# Build and install the library

set(lib_name OpenIGTLinkIF)

add_library(${lib_name}
  ${OpenIGTLinkIF_SRCS} 
  ${OpenIGTLinkIF_TCL_SRCS}
  )
slicer3_set_modules_output_path(${lib_name})


target_link_libraries(${lib_name}
  ${Slicer_Libs_LIBRARIES}
  ${Slicer_Base_LIBRARIES}
  ${KWWidgets_LIBRARIES}
  ${ITK_LIBRARIES}
  ${OpenIGTLink_LIBRARIES}
  )
slicer3_install_modules(${lib_name})

# --------------------------------------------------------------------------
# Testing

if(BUILD_TESTING)
   ADD_SUBDIRECTORY(Testing)
endif(BUILD_TESTING)
