PROJECT(raw2mesh)

# Include this directory before the ITK_DIR. If we are going to make changes
# to the bayesian filters, we want the file in the sandbox to be picked up 
# prior to the one in ITK.
INCLUDE_DIRECTORIES( 
  ${raw2mesh_SOURCE_DIR}
  ${InsightApplications_Source_DIR}/Auxiliary/vtk/
  )

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
ADD_EXECUTABLE(raw2genus0 raw2genus0.cxx )
TARGET_LINK_LIBRARIES(raw2genus0 vtkRendering vtkIO ITKIO ITKCommon ITKBasicFilters ITKStatistics)

# Add the executable
ADD_LIBRARY(linearEqnSolver linearEqnSolver.cxx) 
ADD_EXECUTABLE(genus02mesh genus02mesh.cxx )
TARGET_LINK_LIBRARIES(genus02mesh linearEqnSolver vtkRendering vtkIO ITKCommon ITKBasicFilters ITKIO itkvnl)
