project(Crop)

cmake_minimum_required(VERSION 2.4)

# Slicer 3
find_package(Slicer3 REQUIRED)
include(${Slicer3_USE_FILE})

#Default install prefix
slicer3_set_default_install_prefix_for_external_projects()

set (CLP Crop)
set (${CLP}_SOURCE ${CLP}.cxx)
generateclp(${CLP}_SOURCE ${CLP}.xml)

add_executable(${CLP} ${${CLP}_SOURCE})
slicer3_set_plugins_output_path(${CLP})
target_link_libraries(${CLP} ${ITK_LIBRARIES} MRML vtkITK vtkIO SlicerBaseLogic SlicerBaseCLI)


