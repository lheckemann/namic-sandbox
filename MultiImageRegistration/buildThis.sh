#!/bin/bash
##  This build tool was written by Hans J. Johnson hans-johnson@uiowa.edu

PROJECTNAME=MultiImageRegistration

BUILDNAME=$(uname)_$(uname -m)-$(hostname -s)

SOURCE_DIR=$(dirname $0)
if [ "${SOURCE_DIR}" == "." ]; then
  SOURCE_DIR=$(pwd)
fi

if [ $# -lt 1 ]; then
  ABI="FAST"
else
  ABI=$1
fi
ITK_BUILD_NAME=$(uname).$(uname -m).${ABI}

echo $ABI
## Valid types are Experimental, Continous, Nightly
if [ $# -lt 2 ]; then
  BUILDTYPE=Experimental
else
  BUILDTYPE=$2
fi

CC=/usr/bin/gcc-4.2
CXX=/usr/bin/g++-4.2
if [ ! -f ${CC} ] || [ ! -f ${CXX} ]; then
CC=gcc
CXX=g++
fi
## Removed -Wshadow from build to prevent qt warnings form hiding current errors
#FLAGS_FROM_QT_BUILD="-arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5 -Wall -W "
case ${ABI} in
  "PROFILE")
    CFLAGS="-m64 -Wall -Wstrict-prototypes -fprofile-arcs -ftest-coverage -pg  -UNDEBUG ${FLAGS_FROM_QT_BUILD}"
    CXXFLAGS="-m64 -Wall  -fprofile-arcs -ftest-coverage -pg -UNDEBUG ${FLAGS_FROM_QT_BUILD}"
    ;;
  "OPTDEBUG")
    CFLAGS="-m64 -Wstrict-prototypes -g -O1 ${FLAGS_FROM_QT_BUILD}"
    CXXFLAGS="-m64 -g -O1 ${FLAGS_FROM_QT_BUILD}"
    ;;
  "DEBUG")
    CFLAGS="-m64 -g ${FLAGS_FROM_QT_BUILD}"
    CXXFLAGS="-m64 -g ${FLAGS_FROM_QT_BUILD}"
    ;;
  "FAST")
    CFLAGS="-m64 -DNDEBUG -O3 -msse -mmmx -msse2 -msse3  ${FLAGS_FROM_QT_BUILD}"
    CXXFLAGS="-m64 -DNDEBUG -O3 -msse -mmmx -msse2 -msse3  ${FLAGS_FROM_QT_BUILD}"
    ;;
  *)
    echo "INVALID ABI GIVEN"
    exit -1;
esac
## Auto-determine the number of processors on this computer
case "$(uname)" in
  "Linux")
  maxproc=$(grep processor /proc/cpuinfo | wc | awk '{print $1}')
  ;;
  "Darwin")
  maxproc=$(sysctl -n hw.ncpu)
  ;;
  *)
  echo "Platform not recognized"
  maxproc=1;
esac
if [ "x${maxproc}" == "x" ] || [ ${maxproc} -lt 1 ]; then
  maxproc=1;
fi

COMPILE_DIR=$(dirname ${SOURCE_DIR})/${PROJECTNAME}-COMPILE/
ABI_DIR=${COMPILE_DIR}/$(uname)_$(uname -m)-${ABI}
mkdir -p ${ABI_DIR}

if [ 1 == 1 ];then  ## Temporary bypass of building ITK
  LOCAL_PATH=$(dirname $0)
  echo "${LOCAL_PATH}"
  if [ "${LOCAL_PATH}" == "." ]; then
    LOCAL_PATH=$(pwd)
  fi
  echo "${LOCAL_PATH}"
  #################################################################################
  #Get and build InsightToolkit-CVS
  ITK_SOURCE=${COMPILE_DIR}/Insight
  ITK_BUILD=${ABI_DIR}/InsightToolkit-CVS-build
  if [ ! -f ${ITK_SOURCE}/CMakeLists.txt ] || [ ${LOCAL_PATH}/build${PROJECTNAME}.sh -nt ${ITK_SOURCE}/CMakeLists.txt ]; then
    mkdir -p ${ITK_SOURCE}
    pushd ${COMPILE_DIR}
    cvs -d :pserver:anoncvs:@www.itk.org:/cvsroot/Insight login
    cvs -d :pserver:anoncvs@www.itk.org:/cvsroot/Insight checkout -D 2010-01-26 Insight
    popd
  fi
  mkdir -p ${ITK_BUILD}
  pushd ${ITK_BUILD}
  echo $PWD
  echo ${ITK_SOURCE}
  ##NOTE:  Using cmake and all comand line options.  Normally ccmake would be used.
  CC=${CC} CXX=${CXX} CFLAGS=${CFLAGS} CXXFLAGS=${CXXFLAGS} cmake ${ITK_SOURCE} \
    -DBUILD_EXAMPLES:BOOL=OFF \
    -DBUILD_TESTING:BOOL=OFF \
    -DBUILD_SHARED_LIBS:BOOL=OFF \
    -DBUILDNAME:STRING=${ITK_BUILD_NAME} \
    -DCOVERAGE_COMMAND:FILEPATH=/usr/bin/gcov-4.2 \
    -DITK_USE_ORIENTED_IMAGE_DIRECTION:BOOL=ON \
    -DITK_USE_REVIEW:BOOL=ON \
    -DITK_USE_REVIEW_STATISTICS:BOOL=ON \
    -DITK_USE_TRANSFORM_IO_FACTORIES:BOOL=ON \
    -DITK_USE_OPTIMIZED_REGISTRATION_METHODS:BOOL=ON \
    -DITK_USE_ORIENTED_IMAGE_DIRECTION:BOOL=ON \
    -DITK_IMAGE_BEHAVES_AS_ORIENTED_IMAGE:BOOL=ON
  if [ $? -ne 0 ]; then
    echo "ERROR in configuring ITK"
    exit -1
  fi
  make -j${maxproc}
  popd
fi  ## Temporary bypass of building ITK

#################################################################################
#Build ${PROJECTNAME}
APP_DIR=${ABI_DIR}/${PROJECTNAME}
mkdir -p ${APP_DIR}
pushd ${APP_DIR}
##NOTE:  Using cmake and all comand line options.  Normally ccmake would be used.
CMAKE_MODULE_PATH=${LOCAL_PATH}/CMake CC=${CC} CXX=${CXX} CFLAGS=${CFLAGS} CXXFLAGS=${CXXFLAGS} cmake ${SOURCE_DIR}/ -DBUILD_EXAMPLES:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_SHARED_LIBS:BOOL=OFF -DUSE_PRIVATE:BOOL=ON -DCOMPILE_ITKEMS:BOOL=ON -DVTK_DIR:PATH=${VTK_BUILD} -DITK_DIR:PATH=${ITK_BUILD} -DABI:STRING=${ABI} -DBUILDNAME:STRING=${ITK_BUILD_NAME} -DCOVERAGE_COMMAND:FILEPATH=/usr/bin/gcov-4.2 -DUSE_ITK_LIBRARY:BOOL=ON -DUSE_VTK_LIBRARY:BOOL=ON \
                  -DCOMPILE_DISPLAY:BOOL=OFF \
                  -DBUILD_TESTING:BOOL=ON \

## NOTE: There is an interaction between COMPILE_DISPLAY and USE_QT_LIBRARY


case "$(uname)" in
  "Linux")
  maxproc=$(grep processor /proc/cpuinfo | wc | awk '{print $1}')
  ;;
  "Darwin")
  maxproc=$(sysctl -n hw.ncpu)
  ;;
  *)
  echo "Platform not recognized"
  maxproc=1;
esac
make
popd


