
# Include itk
INCLUDE (${CMAKE_ROOT}/Modules/FindITK.cmake)
INCLUDE(${USE_ITK_FILE})  

# Include Qt
#INCLUDE (${CMAKE_ROOT}/Modules/FindQt.cmake)
INCLUDE (qtSetup.cmake)

# Include OpenGL
INCLUDE (${CMAKE_ROOT}/Modules/FindOpenGL.cmake)

# add the  fltk and open gl libs & includes 
INCLUDE_DIRECTORIES( 
${OPENGL_INCLUDE_PATH} 
)

LINK_LIBRARIES ( 
${OPENGL_LIBRARY} 
)

INCLUDE_DIRECTORIES( 
 ${QT_INCLUDE_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

LINK_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}
)


SET(surf2vol_SRCS
 surf2vol.cxx
 convertBYU.cxx
 Image3D.cxx
 Vector3D.cxx
 RAWImageFile.cxx
 prob.cxx
 surf2volGUIControls.cxx
 raw3io.cxx
 CommandLineArgumentParser.cxx
)

SET(surf2vol_UI_SRCS
  surf2volGUI.ui
)

IF(QT_WRAP_UI)
  QT_WRAP_UI(surf2vol surf2vol_HDRS  surf2vol_SRCS  ${surf2vol_UI_SRCS})
ENDIF(QT_WRAP_UI)

ADD_DEFINITIONS(-DQT_DLL)
ADD_DEFINITIONS(-DQT_THREAD_SUPPORT)

ADD_EXECUTABLE(surf2vol ${surf2vol_SRCS})

TARGET_LINK_LIBRARIES(surf2vol 
 ${QT_QT_LIBRARY}
 ${QT_QTMAIN_LIBRARY}
 ${ITK_LIBRARIES}
)
