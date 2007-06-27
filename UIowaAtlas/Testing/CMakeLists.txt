#Change PROJECT_NAME to the name of your project
PROJECT(volumeRender)


#Declare any external dependencies that your project may have here.
#examples include: ITK, VTK, JPEG, PNG, OpenGL, ZLIB, Perl, Java
#If you're not sure what name to use, look in the Modules directory of your
#cmake install and check that a file named Find(Package).cmake exists
FIND_PACKAGE(VTK REQUIRED)
IF(VTK_FOUND)
  INCLUDE(${VTK_USE_FILE})
ENDIF(VTK_FOUND)


FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)


#Set any libraries that your project depends on.
#examples: ITKCommon, VTKRendering, etc
SET(Libraries
    ITKCommon
    ITKBasicFilters
    ITKIO
    vtkRendering
    vtkGraphics
    vtkHybrid
    vtkImaging
    vtkIO
    vtkFiltering
    vtkCommon
    vtkWidgets
    vtkVolumeRendering
)


ADD_EXECUTABLE(volumeRender volumeRender.cxx)

TARGET_LINK_LIBRARIES(volumeRender ${Libraries})

