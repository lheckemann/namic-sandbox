PROJECT(WFOptionsDialog)

SET(SRCS
WFOptionsDialog.cxx
)


# Find the WFBaseEngine Library

#FIND_LIBRARY(WFENGINE_LIBRARY NAMES WFBaseEngine 
#  PATHS  ../bin /usr/lib /usr/local/lib)
#IF (WFENGINE_LIBRARY)
#  TARGET_ADD_LIBRARY (WFEConfigEditor WFENGINE_LIBRARY)
#ENDIF (WFENGINE_LIBRARY)

INCLUDE (${CMAKE_ROOT}/Modules/FindTCL.cmake)

INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)

## KWWidgets
FIND_PACKAGE(KWWidgets REQUIRED)
IF (NOT KWWidgets_FOUND)
  MESSAGE (FATAL_ERROR "Need KWWidgets to build")
ENDIF (NOT KWWidgets_FOUND)

INCLUDE(${KWWidgets_USE_FILE})

# We define several classes in this example, and we want to be able to use
# their C++ methods as callbacks for our user interface. To do so, we need to 
# create a library and wrap it automatically for the Tcl language, which
# is used as a bridge between C++ objects at run-time. Note that an
# initialization function is automatically created in this library to allow
# classes and C++ methods to be used as commands and callbacks inside the Tcl
# interpreter; do *not* forget to call this function right after you 
# initialize the Tcl interpreter in your application. The name of this 
# function is built from the library name in lower-case (except for the first
# letter) and suffixed with "_Init" (for example: 
# KWWizardDialogExampleLib => Kwwizarddialogexamplelib_Init)
# This whole process is required to use C++ methods as callbacks; it is not
# needed if you use VTK's C++ command/observer pattern directly, which is
# demonstrated in KWWidgets's C++ 'Callbacks' example.

SET(LIB_NAME "WFTestWizardLib")
SET(LIB_SRCS 
  vtkWFTestWizard.cxx
  )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

INCLUDE("${KWWidgets_CMAKE_DIR}/KWWidgetsWrappingMacros.cmake")
KWWidgets_WRAP_TCL(${LIB_NAME} LIB_TCL_SRCS "${LIB_SRCS}" "")

# Create the library. The library is built in static mode for convenience. 
# Check the 'Callbacks' example for more information about building it in
# shared mode, i.e. without the STATIC keyword (Win32 compilers requires
# an additional header file to setup DLL export symbols correctly).

ADD_LIBRARY(${LIB_NAME} STATIC ${LIB_TCL_SRCS} ${LIB_SRCS})
TARGET_LINK_LIBRARIES(${LIB_NAME} ${KWWidgets_LIBRARIES})

# This example uses some files from the KWWidgets distribution tree.
# Let's configure KWWidgets's vtkKWWidgetsPaths.h.in into our
# own header file so that we can find the paths to KWWidgets files.

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
CONFIGURE_FILE(
  ${KWWidgets_TEMPLATES_DIR}/vtkKWWidgetsPaths.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkKWWidgetsPaths.h)

ADD_EXECUTABLE(WFOptionsDialog ${SRCS})
TARGET_LINK_LIBRARIES(WFOptionsDialog ${LIB_NAME})

