PROJECT(WrapVJ)

FIND_PACKAGE(CableSwig REQUIRED)
FIND_PACKAGE(TCL)
INCLUDE_DIRECTORIES(${TCL_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIRS} ${VTK_SOURCE_DIR})
ADD_DEFINITIONS(-DSWIG_GLOBAL)

IF(TCL_TK_STATIC OR VTK_TCL_TK_STATIC)
  ADD_DEFINITIONS(-DSTATIC_BUILD)
ENDIF(TCL_TK_STATIC OR VTK_TCL_TK_STATIC)

IF(NOT BUILD_SHARED_LIBS)
  ADD_DEFINITIONS(-DSTATIC_LINKED) # for Swig
ENDIF(NOT BUILD_SHARED_LIBS)

SET(SWIG_INC -I${VJ_SOURCE_DIR} -I${VJ_BINARY_DIR})
FOREACH(d ${VTK_INCLUDE_DIRS})
  SET(SWIG_INC ${SWIG_INC} -I${d})
ENDFOREACH(d)

IF(NOT KWUtilities_INSTALL_BIN_DIR)
  SET(KWUtilities_INSTALL_BIN_DIR "/bin")
ENDIF(NOT KWUtilities_INSTALL_BIN_DIR)
IF(NOT KWUtilities_INSTALL_LIB_DIR)
  SET(KWUtilities_INSTALL_LIB_DIR "/lib/${PROJECT_NAME}")
ENDIF(NOT KWUtilities_INSTALL_LIB_DIR)

# Indent Macro ObjectFactory SmartPointerBase StdString TimeStamp

SET(VJ_WRAP_CLASSES
  Actor
  Assembly
  Billboard
  Boolean
  Box
  Button
  SignPost
  Camera
  Color
  Function
  FunctionValue
  Icon
  Image
  InterpolatingCurve
  Light
  Link
  LookupTable
  Matrix4x4
  Object
  ObjectBase
  OverlayText
  Path
  PathActor
  PathProperty
  Point
  PolyLineCurve
  Portal
  Scalar
  SimpleText
  SplineCurve
  Surface
  SurfaceProperty
  TemporalObject
  TextProperty
  TimeLine
  Transform
  Vector
  Viewport
  VisualJournal
  Volume
  VolumeProperty
  VTKBillboard
  VTKButton
  VTKSignPost
  VTKCamera
  VTKImage
  VTKLight
  VTKLookupTable
  VTKOverlayText
  VTKPathActor
  VTKPortal
  VTKSplineCurve
  VTKSurface
  VTKSurfaceProperty
  VTKTextProperty
  VTKTransform
  VTKViewport
  VTKVolume
  VTKVolumeProperty
#   XMLActorElementIO
#   XMLAssemblyElementIO
#   XMLBillboardElementIO
#   XMLBooleanElementIO
#   XMLBoxElementIO
#   XMLButtonElementIO
#   XMLCameraElementIO
#   XMLColorElementIO
#   XMLDataElement
#   XMLDataParser
#   XMLElementIOBase
#   XMLFunctionElementIO
#   XMLIDMap
#   XMLIOBase
#   XMLIconElementIO
#   XMLImageElementIO
#   XMLLightElementIO
#   XMLLinkElementIO
#   XMLLookupTableElementIO
#   XMLMatrix4x4ElementIO
#   XMLOverlayTextElementIO
#   XMLParser
#   XMLPathActorElementIO
#   XMLPathElementIO
#   XMLPathPropertyElementIO
#   XMLPointElementIO
#   XMLPortalElementIO
#   XMLReader
#   XMLScalarElementIO
#   XMLSimpleTextElementIO
#   XMLSurfaceElementIO
#   XMLSurfacePropertyElementIO
#   XMLTextPropertyElementIO
#   XMLTimeLineElementIO
#   XMLTransformElementIO
#   XMLUtilities
#   XMLVectorElementIO
#   XMLViewportElementIO
#   XMLVisualJournalElementIO
#   XMLVolumeElementIO
#   XMLVolumePropertyElementIO
#   XMLWriter
  )

SET(VJ_WRAP_SRCS wrap_VJTcl)

FOREACH(c ${VJ_WRAP_CLASSES})
  SET(VJ_CLASS ${c})
  CONFIGURE_FILE(${WrapVJ_SOURCE_DIR}/wrap_vj.cxx.in
                 ${WrapVJ_BINARY_DIR}/wrap_vj${c}.cxx @ONLY IMMEDIATE)
  CONFIGURE_FILE(${WrapVJ_SOURCE_DIR}/wrap_vj.i.in
                 ${WrapVJ_BINARY_DIR}/wrap_vj${c}.i @ONLY IMMEDIATE)
  SET(VJ_WRAP_SRCS ${VJ_WRAP_SRCS} wrap_vj${c})
ENDFOREACH(c)
CONFIGURE_FILE(${WrapVJ_SOURCE_DIR}/wrap_VJTcl.cxx
               ${WrapVJ_BINARY_DIR}/wrap_VJTcl.cxx @ONLY IMMEDIATE)

CONFIGURE_FILE(${WrapVJ_SOURCE_DIR}/wrap_VJTcl.i
               ${WrapVJ_BINARY_DIR}/wrap_VJTcl.i @ONLY IMMEDIATE)

#-----------------------------------------------------------------------------
IF(VJ_BUILD_UTILITY_LIBS)
  ADD_CUSTOM_COMMAND(
    OUTPUT ${WrapVJ_BINARY_DIR}/swigrun.xml
    COMMAND ${CableSwig_gccxml_EXE}
    ARGS -fxml-start=_cable_ -DCABLE_CONFIGURATION
         ${WrapVJ_SOURCE_DIR}/swigrun.cxx
         -fxml=${WrapVJ_BINARY_DIR}/swigrun.xml
    DEPENDS ${WrapVJ_SOURCE_DIR}/swigrun.cxx ${CableSwig_gccxml_EXE}
    )
  ADD_CUSTOM_COMMAND(
    OUTPUT ${WrapVJ_BINARY_DIR}/swigrunTcl.cxx
    COMMAND ${CableSwig_cswig_EXE}
    ARGS -runtime -o ${WrapVJ_BINARY_DIR}/swigrunTcl.cxx -tcl
    -c++ ${WrapVJ_BINARY_DIR}/swigrun.xml
    DEPENDS ${CableSwig_cswig_EXE} ${WrapVJ_BINARY_DIR}/swigrun.xml
    )

  ADD_LIBRARY(SwigRuntimeTcl ${WrapVJ_BINARY_DIR}/swigrunTcl.cxx)
  #SET_TARGET_PROPERTIES(SwigRuntimeTcl PROPERTIES LINK_FLAGS "${CSWIG_EXTRA_LINKFLAGS}")

  IF(TCL_LIBRARY_DEBUG)
    TARGET_LINK_LIBRARIES(SwigRuntimeTcl optimized ${TCL_LIBRARY} debug ${TCL_LIBRARY_DEBUG})
  ELSE(TCL_LIBRARY_DEBUG)
    TARGET_LINK_LIBRARIES(SwigRuntimeTcl ${TCL_LIBRARY})
  ENDIF(TCL_LIBRARY_DEBUG)

  IF(NOT KWUtilities_INSTALL_NO_RUNTIME)
    IF(BUILD_SHARED_LIBS)
      INSTALL_TARGETS(${KWUtilities_INSTALL_LIB_DIR} RUNTIME_DIRECTORY ${KWUtilities_INSTALL_BIN_DIR} SwigRuntimeTcl)
    ENDIF(BUILD_SHARED_LIBS)
  ENDIF(NOT KWUtilities_INSTALL_NO_RUNTIME)
ENDIF(VJ_BUILD_UTILITY_LIBS)

#-----------------------------------------------------------------------------
FOREACH(src ${VJ_WRAP_SRCS})
  #
  # Derive primary_header file name from ${src}
  # to get better dependency rules...
  #
  # Specifically exclude wrap_VJTcl, only include
  # ${src} values that also appear in VJ_WRAP_CLASSES
  # here... The intent is to depend on headers that
  # follow the naming convention:
  #   ${VJ_SOURCE_DIR}/vj/vj${class_name}.h
  #
  SET(primary_header "")
  IF(NOT "${src}" MATCHES "^wrap_VJTcl$")
    STRING(REGEX REPLACE "wrap_vj(.*)$" "\\1" class_name "${src}")
    IF("${class_name}" MATCHES "^$")
      MESSAGE(SEND_ERROR "Could not extract class_name from src='${src}'...")
    ELSE("${class_name}" MATCHES "^$")
      SET(primary_header "${VJ_SOURCE_DIR}/vj/vj${class_name}.h")
      IF(NOT EXISTS "${primary_header}")
        MESSAGE(SEND_ERROR "Can't establish wrapper dependency. Header file '${primary_header}' doesn't exist...")
      ENDIF(NOT EXISTS "${primary_header}")
    ENDIF("${class_name}" MATCHES "^$")
  ENDIF(NOT "${src}" MATCHES "^wrap_VJTcl$")

  ADD_CUSTOM_COMMAND(
    OUTPUT ${WrapVJ_BINARY_DIR}/${src}.xml
    COMMAND ${CableSwig_gccxml_EXE}
    ARGS -fxml=${WrapVJ_BINARY_DIR}/${src}.xml
         -fxml-start=_cable_
         ${SWIG_INC} -DCABLE_CONFIGURATION
         --gccxml-compiler ${CMAKE_CXX_COMPILER}
         ${WrapVJ_BINARY_DIR}/${src}.cxx
    DEPENDS ${WrapVJ_BINARY_DIR}/${src}.cxx ${primary_header}
    )
ENDFOREACH(src)

#-----------------------------------------------------------------------------
SET(VJ_IDX_FILES)
FOREACH(src ${VJ_WRAP_SRCS})
  SET(VJ_IDX_FILES ${VJ_IDX_FILES} ${WrapVJ_BINARY_DIR}/${src}.idx)
  ADD_CUSTOM_COMMAND(
    OUTPUT ${WrapVJ_BINARY_DIR}/${src}.idx
    COMMAND ${CableSwig_cableidx_EXE}
    ARGS \"${WrapVJ_BINARY_DIR}/${src}.xml\" \"${WrapVJ_BINARY_DIR}/${src}.idx\"
    DEPENDS ${WrapVJ_BINARY_DIR}/${src}.xml
    )
ENDFOREACH(src)

#-----------------------------------------------------------------------------
SET(MASTER_IDX_CONTENT)
FOREACH(idx ${VJ_IDX_FILES})
  SET(MASTER_IDX_CONTENT "${MASTER_IDX_CONTENT}${idx}\n")
ENDFOREACH(idx)
CONFIGURE_FILE(${WrapVJ_SOURCE_DIR}/master.idx.in
               ${WrapVJ_BINARY_DIR}/master.idx @ONLY IMMEDIATE)

#-----------------------------------------------------------------------------
SET(IGNORE_WARNINGS -w362 -w389 -w503 -w508 -w509 -w516)
SET(VJ_CXX_FILES)
FOREACH(src ${VJ_WRAP_SRCS})
  SET(VJ_CXX_FILES ${VJ_CXX_FILES} ${WrapVJ_BINARY_DIR}/${src}Tcl.cxx)
  ADD_CUSTOM_COMMAND(
    OUTPUT ${WrapVJ_BINARY_DIR}/${src}Tcl.cxx
    COMMAND ${CableSwig_cswig_EXE}
    ARGS -I${CableSwig_DEFAULT_LIB}
         -I${CableSwig_DEFAULT_LIB}/tcl
         -l\"${WrapVJ_BINARY_DIR}/${src}.i\"
         -noruntime -Cindex ${WrapVJ_BINARY_DIR}/master.idx ${IGNORE_WARNINGS}
         #-depend ${Bin}/${InputXml}.depend
         -o ${WrapVJ_BINARY_DIR}/${src}Tcl.cxx
         -tcl -pkgversion "${VJ_VERSION_STRING}"
         -c++ ${WrapVJ_BINARY_DIR}/${src}.xml
    DEPENDS ${VJ_IDX_FILES} ${WrapVJ_BINARY_DIR}/${src}.xml
            ${CableSwig_cswig_EXE} ${VJ_BINARY_DIR}/vj/vjConfigure.h
    )
ENDFOREACH(src)

#-----------------------------------------------------------------------------
ADD_LIBRARY(VJTcl ${VJ_CXX_FILES})
TARGET_LINK_LIBRARIES(VJTcl VJ SwigRuntimeTcl)

IF(NOT KWUtilities_INSTALL_NO_RUNTIME)
  IF(BUILD_SHARED_LIBS)
    INSTALL_TARGETS(${KWUtilities_INSTALL_LIB_DIR} RUNTIME_DIRECTORY ${KWUtilities_INSTALL_BIN_DIR} VJTcl)
  ENDIF(BUILD_SHARED_LIBS)
ENDIF(NOT KWUtilities_INSTALL_NO_RUNTIME)

#-----------------------------------------------------------------------------
# Configure pkgIndex.tcl for the build tree.
SET(VJ_CSWIG_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES})
IF(VJ_CSWIG_CONFIGURATION_TYPES)
  FOREACH(config ${VJ_CSWIG_CONFIGURATION_TYPES})
    SET(VJ_CSWIG_PACKAGE_DIR ${LIBRARY_OUTPUT_PATH}/${config})
    CONFIGURE_FILE(${WrapVJ_SOURCE_DIR}/pkgIndex.tcl.in
                   ${WrapVJ_BINARY_DIR}/${config}/pkgIndex.tcl
                   @ONLY IMMEDIATE)
  ENDFOREACH(config)
ELSE(VJ_CSWIG_CONFIGURATION_TYPES)
  SET(VJ_CSWIG_PACKAGE_DIR ${LIBRARY_OUTPUT_PATH})
  CONFIGURE_FILE(${WrapVJ_SOURCE_DIR}/pkgIndex.tcl.in
                 ${WrapVJ_BINARY_DIR}/pkgIndex.tcl
                 @ONLY IMMEDIATE)
ENDIF(VJ_CSWIG_CONFIGURATION_TYPES)

#-----------------------------------------------------------------------------
# Configure pkgIndex.tcl for the install tree.
SET(VJ_CSWIG_PACKAGE_DIR "[file dirname [file dirname [info script]]]")
CONFIGURE_FILE(${WrapVJ_SOURCE_DIR}/pkgIndex.tcl.in
               ${WrapVJ_BINARY_DIR}/InstallOnly/Hide/pkgIndex.tcl
               @ONLY IMMEDIATE)
