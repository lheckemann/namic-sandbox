PROJECT(WrapMRML)

FIND_PACKAGE(CableSwig REQUIRED)
FIND_PACKAGE(TCL)
INCLUDE_DIRECTORIES(${MRML_SOURCE_DIR})
INCLUDE_DIRECTORIES(${TCL_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIRS} ${VTK_SOURCE_DIR})
ADD_DEFINITIONS(-DSWIG_GLOBAL)

IF(TCL_TK_STATIC OR VTK_TCL_TK_STATIC)
  ADD_DEFINITIONS(-DSTATIC_BUILD)
ENDIF(TCL_TK_STATIC OR VTK_TCL_TK_STATIC)

IF(NOT BUILD_SHARED_LIBS)
  ADD_DEFINITIONS(-DSTATIC_LINKED) # for Swig
ENDIF(NOT BUILD_SHARED_LIBS)

SET(SWIG_INC -I${MRML_SOURCE_DIR} -I${MRML_BINARY_DIR})
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

SET(MRML_WRAP_CLASSES
  Object
  Collection
  )

SET(MRML_WRAP_SRCS wrap_MRMLTcl)

# Construct the list of groups (cf. wrapMRMLTcl.cxx.in)
FOREACH(c ${MRML_WRAP_CLASSES})
  SET(MRML_CLASS ${c})
  SET(MRML_WRAP_CLASSES_GROUP "${MRML_WRAP_CLASSES_GROUP}\n    ${c},")
  CONFIGURE_FILE(${WrapMRML_SOURCE_DIR}/wrap_mrml.cxx.in
                 ${WrapMRML_BINARY_DIR}/wrap_mrml${c}.cxx @ONLY IMMEDIATE)
  CONFIGURE_FILE(${WrapMRML_SOURCE_DIR}/wrap_mrml.i.in
                 ${WrapMRML_BINARY_DIR}/wrap_mrml${c}.i @ONLY IMMEDIATE)
  SET(MRML_WRAP_SRCS ${MRML_WRAP_SRCS}  wrap_mrml${c})
ENDFOREACH(c)
# Finish the groups with a NULL
SET(MRML_WRAP_CLASSES_GROUP "${MRML_WRAP_CLASSES_GROUP}\n    NULL")

# Configure file that contains the list of wrapped objects:
CONFIGURE_FILE(${WrapMRML_SOURCE_DIR}/wrap_MRMLTcl.cxx.in
               ${WrapMRML_BINARY_DIR}/wrap_MRMLTcl.cxx @ONLY IMMEDIATE)

# Simply copy file:
CONFIGURE_FILE(${WrapMRML_SOURCE_DIR}/wrap_MRMLTcl.i
               ${WrapMRML_BINARY_DIR}/wrap_MRMLTcl.i @ONLY IMMEDIATE)

#-----------------------------------------------------------------------------
SET(MRML_BUILD_UTILITY_LIBS ON)
IF(MRML_BUILD_UTILITY_LIBS)
  ADD_CUSTOM_COMMAND(
    OUTPUT ${WrapMRML_BINARY_DIR}/swigrun.xml
    COMMAND ${CableSwig_gccxml_EXE}
    ARGS -fxml-start=_cable_ -DCABLE_CONFIGURATION
         ${WrapMRML_SOURCE_DIR}/swigrun.cxx
         -fxml=${WrapMRML_BINARY_DIR}/swigrun.xml
    DEPENDS ${WrapMRML_SOURCE_DIR}/swigrun.cxx ${CableSwig_gccxml_EXE}
    )
  ADD_CUSTOM_COMMAND(
    OUTPUT ${WrapMRML_BINARY_DIR}/swigrunTcl.cxx
    COMMAND ${CableSwig_cswig_EXE}
    ARGS -runtime -o ${WrapMRML_BINARY_DIR}/swigrunTcl.cxx -tcl
    -c++ ${WrapMRML_BINARY_DIR}/swigrun.xml
    DEPENDS ${CableSwig_cswig_EXE} ${WrapMRML_BINARY_DIR}/swigrun.xml
    )

  ADD_LIBRARY(SwigRuntimeTcl ${WrapMRML_BINARY_DIR}/swigrunTcl.cxx)
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
ENDIF(MRML_BUILD_UTILITY_LIBS)

#-----------------------------------------------------------------------------
FOREACH(src ${MRML_WRAP_SRCS})
  #
  # Derive primary_header file name from ${src}
  # to get better dependency rules...
  #
  # Specifically exclude wrap_MRMLTcl, only include
  # ${src} values that also appear in MRML_WRAP_CLASSES
  # here... The intent is to depend on headers that
  # follow the naming convention:
  #   ${MRML_SOURCE_DIR}/mrml${class_name}.h
  #
  SET(primary_header "")
  IF(NOT "${src}" MATCHES "^wrap_MRMLTcl$")
    STRING(REGEX REPLACE "wrap_mrml(.*)$" "\\1" class_name "${src}")
    IF("${class_name}" MATCHES "^$")
      MESSAGE(SEND_ERROR "Could not extract class_name from src='${src}'...")
    ELSE("${class_name}" MATCHES "^$")
      SET(primary_header "${MRML_SOURCE_DIR}/mrml${class_name}.h")
      IF(NOT EXISTS "${primary_header}")
        MESSAGE(SEND_ERROR "Can't establish wrapper dependency. Header file '${primary_header}' doesn't exist...")
      ENDIF(NOT EXISTS "${primary_header}")
    ENDIF("${class_name}" MATCHES "^$")
  ENDIF(NOT "${src}" MATCHES "^wrap_MRMLTcl$")

  ADD_CUSTOM_COMMAND(
    OUTPUT ${WrapMRML_BINARY_DIR}/${src}.xml
    COMMAND ${CableSwig_gccxml_EXE}
    ARGS -fxml=${WrapMRML_BINARY_DIR}/${src}.xml
         -fxml-start=_cable_
         ${SWIG_INC} -DCABLE_CONFIGURATION
         --gccxml-compiler ${CMAKE_CXX_COMPILER}
         ${WrapMRML_BINARY_DIR}/${src}.cxx
    DEPENDS ${WrapMRML_BINARY_DIR}/${src}.cxx ${primary_header}
    )
ENDFOREACH(src)

#-----------------------------------------------------------------------------
SET(MRML_IDX_FILES)
FOREACH(src ${MRML_WRAP_SRCS})
  SET(MRML_IDX_FILES ${MRML_IDX_FILES} ${WrapMRML_BINARY_DIR}/${src}.idx)
  ADD_CUSTOM_COMMAND(
    OUTPUT ${WrapMRML_BINARY_DIR}/${src}.idx
    COMMAND ${CableSwig_cableidx_EXE}
    ARGS \"${WrapMRML_BINARY_DIR}/${src}.xml\" \"${WrapMRML_BINARY_DIR}/${src}.idx\"
    DEPENDS ${WrapMRML_BINARY_DIR}/${src}.xml
    )
ENDFOREACH(src)

#-----------------------------------------------------------------------------
SET(MASTER_IDX_CONTENT)
FOREACH(idx ${MRML_IDX_FILES})
  SET(MASTER_IDX_CONTENT "${MASTER_IDX_CONTENT}${idx}\n")
ENDFOREACH(idx)
CONFIGURE_FILE(${WrapMRML_SOURCE_DIR}/master.idx.in
               ${WrapMRML_BINARY_DIR}/master.idx @ONLY IMMEDIATE)

#-----------------------------------------------------------------------------
SET(IGNORE_WARNINGS -w362 -w389 -w503 -w508 -w509 -w516)
SET(MRML_CXX_FILES)
FOREACH(src ${MRML_WRAP_SRCS})
  SET(MRML_CXX_FILES ${MRML_CXX_FILES} ${WrapMRML_BINARY_DIR}/${src}Tcl.cxx)
  ADD_CUSTOM_COMMAND(
    OUTPUT ${WrapMRML_BINARY_DIR}/${src}Tcl.cxx
    COMMAND ${CableSwig_cswig_EXE}
    ARGS -I${CableSwig_DEFAULT_LIB}
         -I${CableSwig_DEFAULT_LIB}/tcl
         -l\"${WrapMRML_BINARY_DIR}/${src}.i\"
         -noruntime -Cindex ${WrapMRML_BINARY_DIR}/master.idx ${IGNORE_WARNINGS}
         #-depend ${Bin}/${InputXml}.depend
         -o ${WrapMRML_BINARY_DIR}/${src}Tcl.cxx
         -tcl -pkgversion "${MRML_VERSION_STRING}"
         -c++ ${WrapMRML_BINARY_DIR}/${src}.xml
    DEPENDS ${MRML_IDX_FILES} ${WrapMRML_BINARY_DIR}/${src}.xml
            ${CableSwig_cswig_EXE} ${MRML_BINARY_DIR}/mrmlConfigure.h
    )
ENDFOREACH(src)

#-----------------------------------------------------------------------------
ADD_LIBRARY(MRMLTcl ${MRML_CXX_FILES})
TARGET_LINK_LIBRARIES(MRMLTcl mrml SwigRuntimeTcl)

IF(NOT KWUtilities_INSTALL_NO_RUNTIME)
  IF(BUILD_SHARED_LIBS)
    INSTALL_TARGETS(${KWUtilities_INSTALL_LIB_DIR} RUNTIME_DIRECTORY ${KWUtilities_INSTALL_BIN_DIR} MRMLTcl)
  ENDIF(BUILD_SHARED_LIBS)
ENDIF(NOT KWUtilities_INSTALL_NO_RUNTIME)

#-----------------------------------------------------------------------------
# Configure pkgIndex.tcl for the build tree.
SET(MRML_CSWIG_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES})
IF(MRML_CSWIG_CONFIGURATION_TYPES)
  FOREACH(config ${MRML_CSWIG_CONFIGURATION_TYPES})
    SET(MRML_CSWIG_PACKAGE_DIR ${LIBRARY_OUTPUT_PATH}/${config})
    CONFIGURE_FILE(${WrapMRML_SOURCE_DIR}/pkgIndex.tcl.in
                   ${WrapMRML_BINARY_DIR}/${config}/pkgIndex.tcl
                   @ONLY IMMEDIATE)
  ENDFOREACH(config)
ELSE(MRML_CSWIG_CONFIGURATION_TYPES)
  SET(MRML_CSWIG_PACKAGE_DIR ${LIBRARY_OUTPUT_PATH})
  CONFIGURE_FILE(${WrapMRML_SOURCE_DIR}/pkgIndex.tcl.in
                 ${WrapMRML_BINARY_DIR}/pkgIndex.tcl
                 @ONLY IMMEDIATE)
ENDIF(MRML_CSWIG_CONFIGURATION_TYPES)

#-----------------------------------------------------------------------------
# Configure pkgIndex.tcl for the install tree.
SET(MRML_CSWIG_PACKAGE_DIR "[file dirname [file dirname [info script]]]")
CONFIGURE_FILE(${WrapMRML_SOURCE_DIR}/pkgIndex.tcl.in
               ${WrapMRML_BINARY_DIR}/InstallOnly/Hide/pkgIndex.tcl
               @ONLY IMMEDIATE)
