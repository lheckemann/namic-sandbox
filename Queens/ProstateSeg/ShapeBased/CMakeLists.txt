cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(projName "test")
project(${projName})
# INCLUDE_REGULAR_EXPRESSION("^.*$")


################################
# include ITK, to avoid setting it everytime, do:
# set the environment variable ITK_DIR to the dir containing ITKConfig.cmake in .bashrc
find_package (ITK)
if (ITK_FOUND)
   include(${USE_ITK_FILE})
endif(ITK_FOUND) 


# boost
find_path(BOOST_PATH /boost/shared_ptr.hpp)
if(NOT BOOST_PATH)
   message(FATAL_ERROR "Please enter the path of boost")
endif(NOT BOOST_PATH)

include_directories(${BOOST_PATH})


include_directories(./lib)

add_library(shapeBasedGAC shapeBasedGAC.cxx)
add_library(shapeBasedSeg shapeBasedSeg.cxx)
add_library(shapeBasedSegCost shapeBasedSegCost.cxx)


set(Libraries
shapeBasedGAC
shapeBasedSeg
shapeBasedSegCost
ITKCommon
ITKNumerics
ITKBasicFilters
ITKIO
itkvnl
)



option(build_wholeSeg "build wholeSeg?" OFF)
if (build_wholeSeg)
    set(cexx wholeSeg)
    ADD_EXECUTABLE(${cexx} ${cexx}.cxx)
    TARGET_LINK_LIBRARIES(${cexx} ${Libraries})
endif (build_wholeSeg)
