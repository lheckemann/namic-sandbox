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


# find cArray path
find_path(CARRAY_PATH cArray2D.h)
if(NOT CARRAY_PATH)
   message(FATAL_ERROR "Please enter the path of cArray.*")
endif(NOT CARRAY_PATH)

include_directories(${CARRAY_PATH})
include_directories(${CARRAY_PATH}/imageProcessing)
include_directories(${CARRAY_PATH}/imageProcessing/sparseFieldLevelSet)


set(Libraries
ITKCommon
ITKNumerics
ITKBasicFilters
ITKIO
itkvnl
)



option(build_meningiomaSegmentor "build meningiomaSegmentor?" OFF)
if (build_meningiomaSegmentor)
    set(cexx meningiomaSegmentor)
    ADD_EXECUTABLE(${cexx} ${cexx}.cxx)
    TARGET_LINK_LIBRARIES(${cexx} ${Libraries})
endif (build_meningiomaSegmentor)
