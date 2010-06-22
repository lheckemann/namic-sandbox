cmake_minimum_required(VERSION 2.6)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(projName "atlasSeg")
project(${projName})

set(CMAKE_CXX_FLAGS -fopenmp)
set(CMAKE_C_FLAGS -fopenmp)


# boost
find_path(BOOST_PATH /boost/shared_ptr.hpp $ENV{BOOST_PATH})
if(NOT BOOST_PATH)
   message(FATAL_ERROR "Please enter the path of boost")
endif(NOT BOOST_PATH)

include_directories(${BOOST_PATH})


################################
# include ITK, to avoid setting it everytime, do:
# set the environment variable ITK_DIR to the dir containing ITKConfig.cmake in .bashrc
#set(ITK_DIR $ENV{ITK_DIR})
find_package(ITK)
if (ITK_FOUND)
   include(${USE_ITK_FILE})
endif(ITK_FOUND) 

set(itkLib
    ITKCommon
    ITKNumerics
    ITKBasicFilters
    ITKIO
    ITKStatistics
    itkvnl)


set(Libraries
    ${itkLib}
)


# # find douher path
# find_path(CARRAY_PATH cArray2D.h $ENV{CARRAY_PATH} REQUIRED)
# if(NOT CARRAY_PATH)
#    message(FATAL_ERROR "Please enter the path of cArray.*")
# endif(NOT CARRAY_PATH)

# include_directories(${CARRAY_PATH})



option(build_atlasSegMIExe "build atlasSegMIExe?" ON)
if (build_atlasSegMIExe)
    set(cexx atlasSegMIExe)
    add_executable(${cexx} ${cexx}.cxx)
    target_link_libraries(${cexx} ${Libraries})
endif (build_atlasSegMIExe)



option(build_atlasSegMIExe_outputPr "build atlasSegMIExe_outputPr?" ON)
if (build_atlasSegMIExe_outputPr)
    set(cexx atlasSegMIExe_outputPr)
    add_executable(${cexx} ${cexx}.cxx)
    target_link_libraries(${cexx} ${Libraries})
endif (build_atlasSegMIExe_outputPr)


option(build_SFLSRobustStat3DTestProbMap "build SFLSRobustStat3DTestProbMap?" ON)
if (build_SFLSRobustStat3DTestProbMap)
    set(cexx SFLSRobustStat3DTestProbMap)
    add_executable(${cexx} ${cexx}.cxx)
    target_link_libraries(${cexx} ${Libraries})
endif (build_SFLSRobustStat3DTestProbMap)
