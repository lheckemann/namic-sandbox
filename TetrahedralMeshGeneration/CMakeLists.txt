# This project is designed to be built outside the Insight source tree.
PROJECT(TetrahedralMeshGeneration)

# Find ITK.
FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

INCLUDE(Dart)
ENABLE_TESTING()

# The VTK ties will be cut eventually...
FIND_PACKAGE(VTK)
IF(VTK_FOUND)
  INCLUDE(${VTK_USE_FILE})
  MESSAGE(${VTK_USE_FILE})
ELSE(VTK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without VTK.  Please set VTK_DIR.")
ENDIF(VTK_FOUND)

SET(PETSC_DIR /usr/local/petsc-2.3.0)
SET(PETSC_ARCH linux-gnu)
SET(X11_DIR /usr/X11R6)
SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "")

INCLUDE_DIRECTORIES(${TetrahedralMeshGeneration_SOURCE_DIR}/BinaryMaskTo3DAdaptiveMeshFilter/Code
                    ${TetrahedralMeshGeneration_SOURCE_DIR}/VTKUnstructuredGridMeshWriter/Code
                    ${TetrahedralMeshGeneration_SOURCE_DIR}/VolumeBoundaryCompressionMeshFilter/Code
                    ${TetrahedralMeshGeneration_SOURCE_DIR}/External # should be removed later
                    ${PETSC_DIR}/include
                   )
LINK_DIRECTORIES(${PETSC_DIR}/lib/${PETSC_ARCH} ${X11_DIR}/lib ${PETSC_DIR}/externalpackages/fblaslapack/${PETSC_ARCH})
                   
FOREACH(test mesher-driver)
      ADD_EXECUTABLE(${test} ${TetrahedralMeshGeneration_SOURCE_DIR}/BinaryMaskTo3DAdaptiveMeshFilter/Tests/${test}.cxx ${TetrahedralMeshGeneration_SOURCE_DIR}/External/predicates.c ${TetrahedralMeshGeneration_SOURCE_DIR}/VolumeBoundaryCompressionMeshFilter/Code/tetra_mesh.cxx)
      TARGET_LINK_LIBRARIES(${test} ITKCommon ITKIO vtkIO vtkGraphics ITKFEM petscksp petscmat petscvec X11 petscsnes petscts nsl rt lamf77mpi util petsc fblas flapack fblas nsl rt util dl lamf77mpi g2c m lam gcc_s)
      ADD_TEST(${test} "${EXECUTABLE_OUTPUT_PATH}/${test}" "sphere" "${TetrahedralMeshGeneration_SOURCE_DIR}/Data/sphere.mhd" "2")
ENDFOREACH(test)
