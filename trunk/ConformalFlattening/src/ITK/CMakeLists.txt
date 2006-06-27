PROJECT(itkConformalFlatteningFilterTest)

FIND_PACKAGE (ITK)
IF (ITK_FOUND)
   INCLUDE(${USE_ITK_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "ITK not found. please set ITK_DIR")
ENDIF(ITK_FOUND) 


FIND_PACKAGE (VTK)
IF (VTK_FOUND)
   INCLUDE(${USE_VTK_FILE})
ELSE(VTK_FOUND)
  MESSAGE(FATAL_ERROR "VTK not found. please set VTK_DIR")
ENDIF(VTK_FOUND) 

ADD_EXECUTABLE(itkConformalFlatteningFilterTest itkConformalFlatteningFilterTest.cxx)
TARGET_LINK_LIBRARIES(itkConformalFlatteningFilterTest  vtkRendering vtkIO ITKCommon ITKBasicFilters ITKIO itkvnl)

