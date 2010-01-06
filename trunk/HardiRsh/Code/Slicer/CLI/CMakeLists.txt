
find_package(GenerateCLP REQUIRED)
if(GenerateCLP_FOUND)
  include(${GenerateCLP_USE_FILE})
endif(GenerateCLP_FOUND)

link_directories(${ModuleDescriptionParser_BINARY_DIR})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

FIND_PATH(SLICER_RESOURCES_DIR ITKLogo.h
  ${Slicer3_DIR}/../../Share/Resources 
  ${Slicer3_DIR}/share/Slicer3/Resources/ )

message("Slicer_Dir = ${Slicer3_DIR}")
message(${SLICER_RESOURCES_DIR})
#####################
set (CLP DiffusionODFEstimation)

set ( ${CLP}_SOURCE ${CLP}.cxx ${SpecialFunctions_SOURCES})
generateclp(${CLP}_SOURCE ${CLP}.xml ${SLICER_RESOURCES_DIR}/ITKLogo.h)
add_executable(${CLP} ${${CLP}_SOURCE})
slicer3_set_plugins_output_path(${CLP})
target_link_libraries ( ${CLP}
  ${Slicer3_Libs_LIBRARIES}
  ${Slicer3_Base_LIBRARIES}
  ${VTK_LIBRARIES}
  ${KWWidgets_LIBRARIES}
  ${ITK_LIBRARIES}
)

include_directories(
  ${Slicer3_Libs_INCLUDE_DIRS}
  ${Slicer3_Base_INCLUDE_DIRS}
)

if (Slicer3_SOURCE_DIR)
  # install each target in the production area (where it would appear in an 
  # installation) and install each target in the developer area (for running 
  # from a build)
  set(TARGETS
    DiffusionODFEstimation
    )

  slicer3_install_plugins(${TARGETS})

endif (Slicer3_SOURCE_DIR)

#

if(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
endif(BUILD_TESTING)
