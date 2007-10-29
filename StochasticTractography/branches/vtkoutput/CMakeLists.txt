PROJECT(StochasticTractography)

SET(CMAKE_VERBOSE_MAKEFILE ON)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR,"ITK not found.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

#INCLUDE(/projects/schiz/software/Slicer3/Libs/GenerateCLP/UseGenerateCLP.cmake)
#INCLUDE(/home/tringo/Meng/Code/subversion/GenerateCLP/UseGenerateCLP.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/../../../GenerateCLP/UseGenerateCLP.cmake)
#INCLUDE(/afs/csail.mit.edu/u/t/tringo/subversion/Slicer3/Libs/GenerateCLP/UseGenerateCLP.cmake)

SET(StochasticTractographyFilter_SOURCE StochasticTractographyFilter.cxx)
GENERATECLP(StochasticTractographyFilter_SOURCE StochasticTractographyFilter.xml)
ADD_EXECUTABLE(StochasticTractographyFilter ${StochasticTractographyFilter_SOURCE})
TARGET_LINK_LIBRARIES(StochasticTractographyFilter ITKIO ITKBasicFilters ITKCommon vtkIO vtkCommon)

SET(CalculateStatistics_SOURCE CalculateStatistics.cxx)
GENERATECLP(CalculateStatistics CalculateStatistics.xml)
ADD_EXECUTABLE(CalculateStatistics ${CalculateStatistics_SOURCE})
TARGET_LINK_LIBRARIES(CalculateStatistics ITKIO ITKBasicFilters ITKCommon vtkIO vtkCommon)

SET(ROITractFilter_SOURCE ROITractFilter.cxx)
GENERATECLP(ROITractFilter ROITractFilter.xml)
ADD_EXECUTABLE(ROITractFilter ${ROITractFilter_SOURCE})
TARGET_LINK_LIBRARIES(ROITractFilter ITKIO ITKBasicFilters ITKCommon vtkIO vtkCommon)

SET(CalculateTractAveragedFA_SOURCE CalculateTractAveragedFA.cxx)
GENERATECLP(CalculateTractAveragedFA CalculateTractAveragedFA.xml)
ADD_EXECUTABLE(CalculateTractAveragedFA ${CalculateTractAveragedFA_SOURCE})
TARGET_LINK_LIBRARIES(CalculateTractAveragedFA ITKIO ITKBasicFilters ITKCommon vtkIO vtkCommon)

SET(ExtractVoxelLikelihood_SOURCE ExtractVoxelLikelihood.cxx)
GENERATECLP(ExtractVoxelLikelihood_SOURCE ExtractVoxelLikelihood.xml)
ADD_EXECUTABLE(ExtractVoxelLikelihood ${ExtractVoxelLikelihood_SOURCE})
TARGET_LINK_LIBRARIES(ExtractVoxelLikelihood ITKIO ITKBasicFilters ITKCommon vtkIO vtkCommon)

SET(EstimateTensorImage_SOURCE EstimateTensorImage.cxx)
GENERATECLP(EstimateTensorImage_SOURCE EstimateTensorImage.xml)
ADD_EXECUTABLE(EstimateTensorImage ${EstimateTensorImage_SOURCE})
TARGET_LINK_LIBRARIES(EstimateTensorImage ITKIO ITKBasicFilters ITKCommon vtkIO vtkCommon)
