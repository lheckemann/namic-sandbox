cmake_minimum_required(VERSION 2.6)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(projName "afibPostAndPreRegistration")
project(${projName})


# Find Slicer3. This will locate all the components needed to build modules 
# for Slicer3.
#
find_package(Slicer3 REQUIRED)
if (Slicer3_FOUND)
  include(${Slicer3_USE_FILE})    
else (Slicer3_FOUND)
  message(ERROR "Cannot build without a Slicer3 build tree or a Slicer3 installation. Please set Slicer3_DIR. If using a Slicer3 build, Slicer3_DIR needs to be set to the root of the build tree of Slicer3.  If using an installation of Slicer3, Slicer3_DIR needs to be set to <SlicerInstallation>/lib/Slicer3.")
endif (Slicer3_FOUND)

set(itkLib ITKCommon ITKNumerics ITKBasicFilters ITKIO ITKStatistics itkvnl)


# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
    "${PROJECT_SOURCE_DIR}/cmakeConfig.h.in"
    "${PROJECT_BINARY_DIR}/cmakeConfig.h"
    )

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

#set(headerAndLibDir ${PROJECT_SOURCE_DIR}/atlas/multiResolutionAffineRegistration)

set(headerAndLibDir
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/lib)

include_directories(${headerAndLibDir})


##################################################
# my libs
# set(myLibSRC
#     ${PROJECT_SOURCE_DIR}/atlas/walltime.cxx)

# add_library(myLib ${myLibSRC})


################################################################################
# stand along exe
################################################################################
option(build_registerPostToPreAffine "build registerPostToPreAffine?" ON)
if (build_registerPostToPreAffine)
    set(cexx registerPostToPreAffine)
    add_executable(${cexx} ${cexx}.cxx)
    target_link_libraries(${cexx} ${itkLib})
endif (build_registerPostToPreAffine)


option(build_registerPostToPreAffineAndDemons "build registerPostToPreAffineAndDemons?" ON)
if (build_registerPostToPreAffineAndDemons)
    set(cexx registerPostToPreAffineAndDemons)
    add_executable(${cexx} ${cexx}.cxx)
    target_link_libraries(${cexx} ${itkLib})
endif (build_registerPostToPreAffineAndDemons)



################################################################################
# Slicer module
################################################################################
set(seg_SRC postToPreRegistration.cxx)
GENERATECLP(seg_SRC postToPreRegistration.xml)
add_executable( postToPreRegistration ${seg_SRC})
target_link_libraries( postToPreRegistration ${itkLib})


#
# Install into a local plugins directory
#
set(TARGETS postToPreRegistration)
foreach(targ ${TARGETS})
  slicer3_set_plugins_output_path(${targ})
endforeach(targ ${TARGETS})
slicer3_install_plugins(${TARGETS})

