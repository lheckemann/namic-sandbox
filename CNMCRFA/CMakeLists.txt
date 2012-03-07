cmake_minimum_required(VERSION 2.8.0)
set(EXTENSION_NAME RFAPlannerCLI)
# Slicer4
find_package(Slicer REQUIRED)
if (NOT Slicer_FOUND)
message(FATAL_ERROR "Slicer Directory not found!")
endif(NOT Slicer_FOUND)
include(${Slicer_USE_FILE})

find_package(SlicerExecutionModel REQUIRED GenerateCLP)
include(${GenerateCLP_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

#find_package(VTK REQUIRED)
#include(${VTK_USE_FILE})


#####################
# Volume Tractography plugin

set (CLP RFAPlanner)
set (${CLP}_SOURCE ${CLP}.cxx)
generateclp(${CLP}_SOURCE ${CLP}.xml)

add_executable(${CLP} ${${CLP}_SOURCE})
#link_directories(${MRML_LIB_DIR} ${vtkTeem_LIB_DIR})
target_link_libraries (${CLP}  ${ITK_LIBRARIES} )
include_directories( ${Slicer_BaseCLI_INCLUDE_DIRS} ${Slicer_SOURCE_DIR}/Base/CLI ${MRML_SOURCE_DIR} ${MRML_BINARY_DIR} ${vtkTeem_SOURCE_DIR} ${vtkTeem_BINARY_DIR})
