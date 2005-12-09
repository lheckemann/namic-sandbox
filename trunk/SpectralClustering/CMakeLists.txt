PROJECT(SpectralClustering)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
        INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
        MESSAGE(FATAL_ERROR "Could not find ITK")
ENDIF(ITK_FOUND)

ADD_LIBRARY( ITKAffinityClustering itkAffinityClustering.cxx )
TARGET_LINK_LIBRARIES( ITKAffinityClustering ITKCommon )

ADD_EXECUTABLE( itkAffinityClusteringTest itkAffinityClusteringTest.cxx )
TARGET_LINK_LIBRARIES( itkAffinityClusteringTest ITKAffinityClustering ITKCommon )

