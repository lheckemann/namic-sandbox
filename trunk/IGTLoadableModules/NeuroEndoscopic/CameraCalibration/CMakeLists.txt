# Configuration of the project
project(NeuroEndoscopicCameraCalibration)

cmake_minimum_required(VERSION 2.4)

# Slicer3
find_package(Slicer3 REQUIRED)
include(${Slicer3_USE_FILE})

# OpenCV
include_directories(${OpenCV_INCLUDE_DIR})
link_directories(${OpenCV_LIB_DIR})


# OpenIGTLink
find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})

# Bin Output path
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

# Default install prefix
#slicer3_set_default_install_prefix_for_external_projects()

#################################
#       FaceDetect Plugin

set(CLP NeuroEndoscopicCameraCalibration)
set(${CLP}_SOURCE NeuroEndoscopicCameraCalibration.cxx)
#generateclp(${CLP}_SOURCE ${CLP}.xml)

add_executable(${CLP} ${${CLP}_SOURCE})
#slicer3_set_plugins_output_path(${CLP})
target_link_libraries(${CLP} OpenIGTLink cv highgui)

#################################

#################################
#       Tests for the plugin

#enable_testing()

#################################

 
