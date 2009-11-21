# same name as the files????
project(MyGrayscaleModelMaker)

cmake_minimum_required(VERSION 2.6)

# Slicer3
find_package(Slicer3 REQUIRED)
include(${Slicer3_USE_FILE})

# Default install prefix
slicer3_set_default_install_prefix_for_external_projects()


# BinaryThresholding plugin
# must be the same name
set (CLP MyGrayscaleModelMaker)
set (${CLP}_SOURCE ${CLP}.cxx)
generateclp(${CLP}_SOURCE ${CLP}.xml)

add_executable(${CLP} ${${CLP}_SOURCE})
slicer3_set_plugins_output_path(${CLP})
target_link_libraries (${CLP}
ITKBasicFilters ITKCommon ITKIO
vtkRendering  vtkGraphics vtkHybrid
vtkImaging vtkIO vtkFiltering vtkCommon
  vtkITK vtkImaging vtkGraphics vtkIO
  ITKIO
  ModuleDescriptionParser
  SlicerBaseCLI
  )
include_directories(${vtkITK_SOURCE_DIR} ${vtkITK_BINARY_DIR})


#####################


#####################
# Tests for the plugin

enable_testing()


#####################
