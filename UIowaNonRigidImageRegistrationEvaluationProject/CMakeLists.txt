#PROJECT NAME
PROJECT( NIREP )

#Search harddrive for itk build
FIND_PACKAGE( ITK REQUIRED )
IF ( ITK_FOUND )
    INCLUDE( ${USE_ITK_FILE} )
ENDIF( ITK_FOUND )

#Search harddrive for kwwidgets build
FIND_PACKAGE( KWWidgets REQUIRED )
IF ( KWWidgets_FOUND )
    INCLUDE(${KWWidgets_USE_FILE})
ENDIF( KWWidgets_FOUND )

#Search harddrive for vtk build
FIND_PACKAGE( VTK REQUIRED )
IF ( VTK_FOUND )
    INCLUDE( ${USE_VTK_FILE} )
ENDIF( VTK_FOUND )

#Insert your source files here *.cxx, *.cpp, *.c and *.cc
SET( NIREP_src NIREPApp.cxx
    vtkKWNIREPGridImageViewer.cxx )

#Create executable using source files
ADD_EXECUTABLE( NIREP ${NIREP_src} )

#Basic itk, vtk and kwwidgets libraries. Note: ORDER IS IMPORTANT.
TARGET_LINK_LIBRARIES( NIREP
    ITKBasicFilters
    ITKCommon
    ITKIO
    KWWidgets
    vtkRendering
    vtkGraphics
    vtkHybrid
    vtkImaging
    vtkIO
    vtkFiltering
    vtkCommon
)
