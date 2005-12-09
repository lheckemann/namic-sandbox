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

#SET(PETSC_DIR /home/scratch/fedorov/local/petsc-2.3.0)
SET(PETSC_DIR ${TetrahedralMeshGeneration_SOURCE_DIR}/PETSc/petsc-2.3.0)
SET(MESH_DIR ${TetrahedralMeshGeneration_SOURCE_DIR}/Mesh-1.13-mod)

SET(PETSC_ARCH linux_gnu)
SET(X11_DIR /usr/X11R6)
SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "")
SET(CMAKE_CXX_FLAGS "-g -Wno-deprecated")

INCLUDE_DIRECTORIES(${TetrahedralMeshGeneration_SOURCE_DIR}/BinaryMaskTo3DAdaptiveMeshFilter/Code
                    ${TetrahedralMeshGeneration_SOURCE_DIR}/VTKUnstructuredGridMeshWriter/Code
                    ${TetrahedralMeshGeneration_SOURCE_DIR}/VolumeBoundaryCompressionMeshFilter/Code
                    ${TetrahedralMeshGeneration_SOURCE_DIR}/External # should be removed later
                    ${TetrahedralMeshGeneration_SOURCE_DIR}/TetrahedralMeshReader/Code
                    ${TetrahedralMeshGeneration_SOURCE_DIR}/TetrahedralMeshWriter/Code
                    ${TetrahedralMeshGeneration_SOURCE_DIR}/TetrahedralMeshToSurfaceMeshFilter/Code
                    ${TetrahedralMeshGeneration_SOURCE_DIR}/TriangularSurfaceMeshWriter/Code
                    ${MESH_DIR}
                    ${MESH_DIR}/lib3d/include
                    ${PETSC_DIR}/include
                   )
LINK_DIRECTORIES(${PETSC_DIR}/lib/${PETSC_ARCH} ${X11_DIR}/lib ${PETSC_DIR}/externalpackages/fblaslapack/${PETSC_ARCH} ${PETSC_DIR}/lib/${PETSC_ARCH} ${MESH_DIR}/lib)
                   
#FOREACH(test mesher-driver mesher-reader)
#      ADD_EXECUTABLE(${test} ${TetrahedralMeshGeneration_SOURCE_DIR}/BinaryMaskTo3DAdaptiveMeshFilter/Tests/${test}.cxx ${TetrahedralMeshGeneration_SOURCE_DIR}/External/predicates.c ${TetrahedralMeshGeneration_SOURCE_DIR}/VolumeBoundaryCompressionMeshFilter/Code/tetra_mesh.cxx)
#      TARGET_LINK_LIBRARIES(${test} ITKCommon ITKIO vtkIO vtkGraphics ITKFEM petscksp petscmat petscvec X11 petscsnes petscts nsl rt lamf77mpi util petsc fblas flapack fblas nsl rt util dl lamf77mpi g2c m lam gcc_s mesh z)
#      ADD_TEST(${test} "${EXECUTABLE_OUTPUT_PATH}/${test}" "sphere" "${TetrahedralMeshGeneration_SOURCE_DIR}/Data/sphere.mhd" "2")
#ENDFOREACH(test)

FOREACH(test mesher-driver mesher-reader mesher-cutter mesh2image vol2raw vtksurf2raw m2mcmp meshqual-driver)
      ADD_EXECUTABLE(${test} ${TetrahedralMeshGeneration_SOURCE_DIR}/Tools/${test}.cxx ${TetrahedralMeshGeneration_SOURCE_DIR}/External/predicates.c ${TetrahedralMeshGeneration_SOURCE_DIR}/VolumeBoundaryCompressionMeshFilter/Code/tetra_mesh.cxx)
      TARGET_LINK_LIBRARIES(${test} ITKCommon ITKIO vtkIO vtkGraphics ITKFEM petscksp petscmat petscvec X11 petscsnes petscts nsl rt lamf77mpi util petsc fblas flapack fblas nsl rt util dl lamf77mpi g2c m lam gcc_s mesh z)
      ADD_TEST(${test} "${EXECUTABLE_OUTPUT_PATH}/${test}" "sphere" "${TetrahedralMeshGeneration_SOURCE_DIR}/Data/sphere.mhd" "2")
ENDFOREACH(test)
