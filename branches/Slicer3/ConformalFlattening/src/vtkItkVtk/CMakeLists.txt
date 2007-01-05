PROJECT(vtkItkVtk)

# Include this directory before the ITK_DIR. If we are going to make changes
# to the bayesian filters, we want the file in the sandbox to be picked up 
# prior to the one in ITK.
#INCLUDE_DIRECTORIES( 
#  ${vtkItkVtk_SOURCE_DIR}
#  ${InsightApplications_Source_DIR}/Auxiliary/vtk/
#  )

SET(VTK_DIR /usr/local/lib/vtk-5.0)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR.")
ENDIF(ITK_FOUND)

FIND_PACKAGE(VTK)
IF(VTK_FOUND)
  INCLUDE(${VTK_USE_FILE})
ELSE(VTK_FOUND)
  MESSAGE(FATAL_ERROR "VTK not found. Please set VTK_DIR.")
ENDIF(VTK_FOUND)

# Add the executable
ADD_EXECUTABLE(vtkItkVtk vtkItkVtk.cxx )
TARGET_LINK_LIBRARIES(vtkItkVtk vtkRendering vtkIO ITKCommon ITKBasicFilters ITKIO)
