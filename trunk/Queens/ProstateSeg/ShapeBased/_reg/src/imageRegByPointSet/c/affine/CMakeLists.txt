CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)


SET(projName "icpTest")
PROJECT(${projName})
# INCLUDE_REGULAR_EXPRESSION("^.*$")

################################
# include ITK, to avoid setting it everytime, do:
# set the environment variable ITK_DIR to the dir containing ITKConfig.cmake in .bashrc
FIND_PACKAGE (ITK)
IF (ITK_FOUND)
   INCLUDE(${USE_ITK_FILE})
ENDIF(ITK_FOUND) 

# # find douher path
# FIND_PATH(DOUHER_PATH cArray2D.h)
# IF(NOT DOUHER_PATH)
#    MESSAGE(FATAL_ERROR "Please enter the path of douher")
# ENDIF(NOT DOUHER_PATH)

# INCLUDE_DIRECTORIES(${DOUHER_PATH})

# boost
FIND_PATH(BOOST_PATH /boost/shared_ptr.hpp)
IF(NOT BOOST_PATH)
   MESSAGE(FATAL_ERROR "Please enter the path of boost")
ENDIF(NOT BOOST_PATH)

INCLUDE_DIRECTORIES(${BOOST_PATH})


SET(itkLibs
ITKCommon
ITKNumerics
ITKBasicFilters
ITKIO
)

#ADD_LIBRARY(gradientDescent ${DOUHER_PATH}/fn/gradientDescent.cxx)


ADD_LIBRARY(kdtree2 ../kdtree2.cpp)
ADD_LIBRARY(pointSetFileIO ../pointSetFileIO.cxx)
ADD_LIBRARY(imageToPointSet ../imageToPointSet.cxx)

ADD_LIBRARY(ICP3 ICP3.cxx)
ADD_LIBRARY(ICPCost3 ICPCost3.cxx)


ADD_LIBRARY(imgRegByPtSet3D imgRegByPtSet3D.cxx)
ADD_LIBRARY(imgRegByPtSet3D_uchar imgRegByPtSet3D_uchar.cxx)

SET(Libraries
    imgRegByPtSet3D_uchar
    imgRegByPtSet3D
    ICP3
    pointSetFileIO
    imageToPointSet
    ICPCost3
    kdtree2
    ${itkLibs}
    )


option(build_imgRegByPtSet3DTest_uchar "build imgRegByPtSet3DTest_uchar?" OFF)
if (build_imgRegByPtSet3DTest_uchar)
    set(cexx imgRegByPtSet3DTest_uchar)
    ADD_EXECUTABLE(${cexx} ${cexx}.cxx)
    TARGET_LINK_LIBRARIES(${cexx} ${Libraries})
endif (build_imgRegByPtSet3DTest_uchar)
