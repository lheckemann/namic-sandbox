PROJECT(SpectralClustering)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
        INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
        MESSAGE(FATAL_ERROR "Could not find ITK")
ENDIF(ITK_FOUND)

FIND_PACKAGE(VTK)
IF(VTK_FOUND)
        INCLUDE(${VTK_USE_FILE})
ELSE(VTK_FOUND)
        MESSAGE(FATAL_ERROR "Could not find VTK")
ENDIF(VTK_FOUND)


ADD_EXECUTABLE(testCluster testCluster.cxx vtkNormalizedCuts.cxx)
TARGET_LINK_LIBRARIES(testCluster ITKCommon ITKNumerics vtkCommon vtkIO vtkImaging vtkFiltering)

ADD_EXECUTABLE(itkKdTreeBasedKmeansEstimatorTest itkKdTreeBasedKmeansEstimatorTest.cxx )
TARGET_LINK_LIBRARIES(itkKdTreeBasedKmeansEstimatorTest ITKCommon ITKNumerics ITKStatistics)

