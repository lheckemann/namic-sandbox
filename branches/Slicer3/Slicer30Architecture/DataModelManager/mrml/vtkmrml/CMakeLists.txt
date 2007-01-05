FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${USE_VTK_FILE})

# vtkmrml lib
SET(VTKMRML_SRCS
  vtkmrmlModel.cxx
  vtkmrmlVolume.cxx
  )

# Add the include paths
INCLUDE_DIRECTORIES(
  ${MRML_SOURCE_DIR}
  )

ADD_LIBRARY(vtkmrml ${VTKMRML_SRCS})
