cmake_minimum_required(VERSION 2.6)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(projName "leftAtriumWallSegmentation")
project(${projName})


# # boost
# find_path(BOOST_PATH /boost/shared_ptr.hpp $ENV{BOOST_PATH})
# if(NOT BOOST_PATH)
#    message(FATAL_ERROR "Please enter the path of boost")
# endif(NOT BOOST_PATH)

# include_directories(${BOOST_PATH})



#
# Find Slicer3. This will locate all the components needed to build modules 
# for Slicer3.
#
find_package(Slicer3 REQUIRED)
if (Slicer3_FOUND)
    include(${Slicer3_USE_FILE})    
else (Slicer3_FOUND)
    message(ERROR "Cannot build without a Slicer3 build tree or a Slicer3 installation. Please set Slicer3_DIR. If using a Slicer3 build, Slicer3_DIR needs to be set to the root of the build tree of Slicer3.  If using an installation of Slicer3, Slicer3_DIR needs to be set to <SlicerInstallation>/lib/Slicer3.")
endif (Slicer3_FOUND)



set(seg_SRC leftAtriumWallSegmenter.cxx)
GENERATECLP(seg_SRC leftAtriumWallSegmenter.xml)
add_executable( leftAtriumWallSegmenterModule ${seg_SRC})
target_link_libraries( leftAtriumWallSegmenterModule ITKCommon ITKNumerics ITKBasicFilters ITKIO ITKStatistics itkvnl)



#
# Install into a local plugins directory
#
set(TARGETS leftAtriumWallSegmenterModule)
foreach(targ ${TARGETS})
slicer3_set_plugins_output_path(${targ})
endforeach(targ ${TARGETS})
slicer3_install_plugins(${TARGETS})

##
# build corresponding console exe for debug
option(build_leftAtriumWallSegmenterConsole "build leftAtriumWallSegmenterConsole?" ON)
if (build_leftAtriumWallSegmenterConsole)
    set(cexx leftAtriumWallSegmenterConsole)
    add_executable(${cexx} ${cexx}.cxx)
    target_link_libraries(${cexx}  ITKCommon ITKNumerics ITKBasicFilters ITKIO ITKStatistics itkvnl)
endif (build_leftAtriumWallSegmenterConsole)


