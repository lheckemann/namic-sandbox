# This project is designed to be built outside the Insight source tree.
PROJECT(fmri_reg)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

# Determine what packages are available
# Add an option to use or not use VTK
OPTION(USE_VTK "Use VTK (The Visualization Toolkit) (some applications need this)" ON)
IF(USE_VTK)
  FIND_PACKAGE(VTK)
  IF (VTK_FOUND)
    INCLUDE (${VTK_USE_FILE})

    # Test for VTK using std libs
    IF(NOT VTK_USE_ANSI_STDLIB)
      MESSAGE("Warning.  Your VTK was not built with the VTK_USE_ANSI_STDLIB "
              "option ON.  Link errors may occur.  Consider re-building VTK "
              "with this option ON.  For MSVC 6, you MUST turn on the VTK "
              "option.  ITK will not link otherwise.")
    ENDIF(NOT VTK_USE_ANSI_STDLIB)

    # Test for VTK building Hybrid
    IF(NOT VTK_USE_HYBRID)
      MESSAGE("Your VTK was not built with the VTK_USE_HYBRID option ON.  "
              "Please reconfigure and recompile VTK with this option before "
              "trying to use it with ITK.")
      SET(VTK_FOUND 0)
    ENDIF(NOT VTK_USE_HYBRID)

    # If VTK was built using Patented, add an option for using patented code
    # in InsightApplications (SNAP in particular)
    IF(VTK_USE_PATENTED)
      OPTION(USE_VTK_PATENTED "Use VTK Patented Libraries (optional)" OFF)
      MARK_AS_ADVANCED(USE_VTK_PATENTED)
      IF(USE_VTK_PATENTED)
        ADD_DEFINITIONS(-DUSE_VTK_PATENTED)
      ENDIF(USE_VTK_PATENTED)
    ENDIF(VTK_USE_PATENTED)

  ENDIF (VTK_FOUND)
ENDIF(USE_VTK)

# Look for OpenGL.
FIND_PACKAGE(OpenGL)

IF(OPENGL_INCLUDE_PATH)
  INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_PATH})
ENDIF(OPENGL_INCLUDE_PATH)

ADD_EXECUTABLE(fmri_reg main.cpp register.cpp types.h includes.h itk2vtk.h)

INCLUDE_DIRECTORIES(${../inc})

TARGET_LINK_LIBRARIES(fmri_reg ITKBasicFilters ITKAlgorithms ITKStatistics ITKNumerics ITKCommon ITKIO vtkVolumeRendering vtkRendering vtkHybrid vtkGraphics vtkImaging vtkIO vtkFiltering vtkCommon)


ADD_EXECUTABLE(ConvertVolumeToSeriesOfSlices ImageReadImageSeriesWrite.cxx)
TARGET_LINK_LIBRARIES(ConvertVolumeToSeriesOfSlices ITKIO)

ADD_EXECUTABLE(ImageRegistration16 ImageRegistration16.cxx)
TARGET_LINK_LIBRARIES(ImageRegistration16 ITKIO ITKNumerics)

