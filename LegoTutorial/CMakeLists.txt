PROJECT(Lego)

IF(NOT USE_IGSTK)
 ADD_DEFINITIONS(-DIGSTK_OFF)
ENDIF(NOT USE_IGSTK)

#
# Sources
#
SET(Lego_SRCS 
  # module
  vtkLegoLogic.cxx
  vtkLegoGUI.cxx
)

SET(CMAKE_CXX_FLAGS
  "-funsigned-char"
)

#
# Include dirs
#
INCLUDE_DIRECTORIES(
  ${ModulesBaseClasses_SOURCE_DIR}
  ${ModulesBaseClasses_BINARY_DIR}
  ${Lego_SOURCE_DIR}
  ${Lego_BINARY_DIR}
  ${SlicerBase_SOURCE_DIR}
  ${SlicerBase_BINARY_DIR}
  ${SlicerBaseLogic_SOURCE_DIR}
  ${SlicerBaseLogic_BINARY_DIR}
  ${SlicerBaseGUI_SOURCE_DIR}
  ${SlicerBaseGUI_BINARY_DIR}
  ${vtkITK_SOURCE_DIR}
  ${vtkITK_BINARY_DIR}
  ${ITK_INCLUDE_DIR}
  ${VTK_INCLUDE_DIR}
  ${MRML_SOURCE_DIR}
  ${MRML_BINARY_DIR}
  ${IGT_SOURCE_DIR}
  ${IGT_BINARY_DIR}
  ${OPENTRACKER_INC_DIR}
  ${IGSTK_INCLUDE_DIRS}
  ./libusb-0.1.12
  )

#
# Wrapping
#
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
VTK_WRAP_TCL3(Lego Lego_TCL_SRCS "${Lego_SRCS}" "")

#
# Build the library
#
ADD_LIBRARY(Lego
  ${Lego_SRCS} 
  ${Lego_TCL_SRCS}
  NXT_USB.cxx
  NXT_USB_linux.cxx
  )

IF(BUILD_SHARED_LIBS)
  INSTALL_TARGETS(${SLICER3_LIB_INSTALL_DIR} Lego)
ENDIF(BUILD_SHARED_LIBS)

FIND_LIBRARY(LIBUSB_LIB
  NAMES libusb-lib
  PATHS
     ./libusb-0.1.12/lib
)

TARGET_LINK_LIBRARIES(Lego
  SlicerBaseLogic 
  SlicerBaseGUI 
  MRML
  vtkITK
  vtkCommonTCL
  vtkImagingTCL
  vtkFilteringTCL
  vtkIOTCL
  ITKAlgorithms
  ITKNumerics 
  ITKCommon
  ITKBasicFilters
  ITKNumerics
  ITKStatistics
  ITKBasicFilters
  ITKIO
  ITKDICOMParser
  ${KWWidgets_LIBRARIES} 
  vtkHybridTCL
  IGT
  ${IGSTK_LIB}
  /home/dpace/Slicer3/Modules/Lego/libusb-0.1.12/lib/libusb.so
  )

#
# Configure include file
#
CONFIGURE_FILE(
  ${Lego_SOURCE_DIR}/vtkLegoConfigure.h.in 
  ${Lego_BINARY_DIR}/vtkLegoConfigure.h
  )
