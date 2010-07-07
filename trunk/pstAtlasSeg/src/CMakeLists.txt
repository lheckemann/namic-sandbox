cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(projName "pcaAnalysis")
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


# add_library(pcaAnalysis ../pcaAnalysis.cxx)
# add_library(imagePCAAnalysis ../imagePCAAnalysis.cxx)

set(Libraries
#imagePCAAnalysis
#pcaAnalysis
    ITKCommon
    ITKNumerics
    ITKBasicFilters
    ITKIO
    itkvnl
    )


option(build_imagePCAAnalysisTest2 "build imagePCAAnalysisTest2?" ON)
if (build_imagePCAAnalysisTest2)
    set(cexx imagePCAAnalysisTest2)
    add_executable(${cexx} ${cexx}.cxx)
    target_link_libraries(${cexx} ${Libraries})
endif (build_imagePCAAnalysisTest2)



option(build_reg_3d_similarity_mse "build reg_3d_similarity_mse?" ON)
if (build_reg_3d_similarity_mse)
    set(cexx reg_3d_similarity_mse)
    add_executable(${cexx} ${cexx}.cxx)
    target_link_libraries(${cexx} ${Libraries})
endif (build_reg_3d_similarity_mse)



option(build_shapeProjectorTest "build shapeProjectorTest?" OFF)
if (build_shapeProjectorTest)
    set(cexx shapeProjectorTest)
    add_executable(${cexx} ${cexx}.cxx)
    target_link_libraries(${cexx} ${Libraries})
endif (build_shapeProjectorTest)



option(build_SFLSRobustStat3DTestProbMap "build SFLSRobustStat3DTestProbMap?" OFF)
if (build_SFLSRobustStat3DTestProbMap)
    set(cexx SFLSRobustStat3DTestProbMap)
    add_executable(${cexx} ${cexx}.cxx)
    target_link_libraries(${cexx} ${Libraries})
endif (build_SFLSRobustStat3DTestProbMap)



option(build_probMapShapeBasedSegExe "build probMapShapeBasedSegExe?" ON)
if (build_probMapShapeBasedSegExe)
    set(cexx probMapShapeBasedSegExe)
    add_executable(${cexx} ${cexx}.cxx)
    target_link_libraries(${cexx} ${Libraries})
endif(build_probMapShapeBasedSegExe)



option(build_atlasSegMIExe_outputPr "build atlasSegMIExe_outputPr?" ON)
if (build_atlasSegMIExe_outputPr)
    set(cexx atlasSegMIExe_outputPr)
    add_executable(${cexx} ${cexx}.cxx)
    target_link_libraries(${cexx} ${Libraries})
endif(build_atlasSegMIExe_outputPr)


