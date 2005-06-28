# This project is designed to be built outside the Insight source tree.
PROJECT(TetrahedralMeshGeneration)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

# The VTK ties will be cut eventually...
FIND_PACKAGE(VTK)
IF(VTK_FOUND)
  INCLUDE(${VTK_USE_FILE})
  MESSAGE(${VTK_USE_FILE})
ELSE(VTK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without VTK.  Please set VTK_DIR.")
ENDIF(VTK_FOUND)

SET(CMAKE_CXX_FLAGS "-g -Wno-deprecated")

SET(NAMIC_SANDBOX_HOME "/scratch/papers/NAMICSandBox/TetrahedralMeshGeneration")

INCLUDE_DIRECTORIES(${NAMIC_SANDBOX_HOME}/BinaryMaskTo3DAdaptiveMeshFilter/Code)
INCLUDE_DIRECTORIES(${NAMIC_SANDBOX_HOME}/VTKUnstructuredGridMeshWriter/Code)
INCLUDE_DIRECTORIES(${NAMIC_SANDBOX_HOME}/External)
# This directory will NOT be a part of ITK, its functionality will be
# reimplemented in a much simpler way within the mesher class
INCLUDE_DIRECTORIES(${NAMIC_SANDBOX_HOME}/External)

ADD_EXECUTABLE(mesher-driver ${NAMIC_SANDBOX_HOME}/BinaryMaskTo3DAdaptiveMeshFilter/Tests/mesher-driver.cxx ${NAMIC_SANDBOX_HOME}/External/predicates.c)

TARGET_LINK_LIBRARIES(mesher-driver ITKCommon ITKIO vtkIO vtkGraphics)
