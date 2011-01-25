# Shared

# Add include directory for itkImageToVTKImageFilter.h and itkVTKImageToImageFilter.h 
INCLUDE_DIRECTORIES ( ${ITK_SOURCE_DIR}/Wrapping/WrapITK/ExternalProjects/ItkVtkGlue/src )
 
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/DefRegEval.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/DefRegEval.h
  )
INCLUDE_DIRECTORIES ( ${CMAKE_CURRENT_BINARY_DIR} )
 
ADD_LIBRARY(dreIO
  IO/vtkLsDynaBinaryPlotReader.cxx 
  IO/vtkLsDynaBinaryPlotReader.h
  IO/vtkMeditMeshReader.cxx 
  IO/vtkMeditMeshReader.h
  IO/vtkNetgenMeshReader.cxx 
  IO/vtkNetgenMeshReader.h
  IO/vtkSMeshWriter.cxx
  IO/vtkSMeshWriter.h
  IO/vtkFEBioWriter.cxx 
  IO/vtkFEBioWriter.h
  )

##########################

ADD_EXECUTABLE(CreateShapeModel 
  CreateShapeModel.cxx
  DefRegEval.cxx
  )
  
TARGET_LINK_LIBRARIES(CreateShapeModel 
  ITKCommon 
  ITKIO 
  ITKBasicFilters 
  ITKAlgorithms 
  vtksys
  )

##########################

ADD_EXECUTABLE(GenerateShapeImage 
  GenerateShapeImage.cxx
  DefRegEval.cxx
  )
  
TARGET_LINK_LIBRARIES(GenerateShapeImage 
  ITKCommon 
  ITKIO 
  ITKBasicFilters 
  ITKAlgorithms 
  vtksys 
  vtkCommon
  )

##########################

ADD_EXECUTABLE(ExtractSurface
  ExtractSurface.cxx  
  DefRegEval.cxx
  )
  
TARGET_LINK_LIBRARIES(ExtractSurface
  vtkIO  
  vtkGraphics
  vtksys
  vtkHybrid
  vtkImaging 
  dreIO
  itksys
  )

##########################

ADD_EXECUTABLE(CreateFemModel 
  CreateFemModel.cxx
  DefRegEval.cxx
  )
  
TARGET_LINK_LIBRARIES(CreateFemModel 
  ITKCommon 
  ITKIO 
  ITKBasicFilters 
  ITKAlgorithms 
  vtksys 
  vtkIO  
  vtkGraphics
  dreIO
  )

##########################

ADD_EXECUTABLE(AddImageSegmentationError 
  AddImageSegmentationError.cxx
  DefRegEval.cxx
  )
  
TARGET_LINK_LIBRARIES(AddImageSegmentationError 
  ITKCommon 
  ITKIO 
  ITKBasicFilters 
  ITKAlgorithms 
  vtksys 
  )

##########################

ADD_EXECUTABLE(GetDeformedImage 
  GetDeformedImage.cxx
  DefRegEval.cxx
  )
  
TARGET_LINK_LIBRARIES(GetDeformedImage  
  vtksys 
  vtkIO  
  vtkGraphics
  vtkImaging
  vtkHybrid 
  ITKCommon 
  ITKIO 
  ITKBasicFilters 
  ITKAlgorithms
  dreIO 
  )
  
##########################

ADD_EXECUTABLE(RegisterImages 
  RegisterImages.cxx
  DefRegEval.cxx
  )
  
TARGET_LINK_LIBRARIES(RegisterImages 
  ITKCommon 
  ITKIO 
  ITKBasicFilters 
  ITKAlgorithms 
  vtksys 
  )
  
##########################

ADD_EXECUTABLE(EvaluateRegistrationError 
  EvaluateRegistrationError.cxx
  DefRegEval.cxx
  )
  
TARGET_LINK_LIBRARIES(EvaluateRegistrationError 
  ITKCommon 
  ITKIO 
  ITKBasicFilters 
  ITKAlgorithms 
  vtksys 
  )
  
##########################

ADD_EXECUTABLE(ComputeMultiImageStatistics 
  ComputeMultiImageStatistics.cxx
  DefRegEval.cxx
  )
  
TARGET_LINK_LIBRARIES(ComputeMultiImageStatistics 
  ITKCommon 
  ITKIO 
  ITKBasicFilters 
  ITKAlgorithms 
  vtksys 
  )
