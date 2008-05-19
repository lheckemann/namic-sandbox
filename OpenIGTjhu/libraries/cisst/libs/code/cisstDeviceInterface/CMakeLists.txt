#
# $Id: CMakeLists.txt,v 1.18 2006/05/07 04:45:26 kapoor Exp $ 
#
# CMakeLists for cisstDeviceInterface
#

# set the library name
SET(LIBRARY cisstDeviceInterface)

# set dependencies
SET(DEPENDENCIES
    cisstCommon
    cisstOSAbstraction
    )

# all source files
SET(SOURCE_FILES
    ddiDeviceInterface.cpp
    ddiCompositeDevice.cpp
    ddiCompositeCommand0.cpp
    ddiCompositeCommand1.cpp
    ddiCompositeCommand2.cpp
    ddiNOPS.cpp
    ddiTypes.cpp

    ddiATIDAQFT.cpp
    ddiATIDAQFTLowLevelIO.cpp
    ddiATIISAFT.cpp
    ddiATIISAFTLowLevelIO.cpp

    ddiLoPoMoCo.cpp
    ddiTwoLoPoMoCo.cpp
    ddiThreeLoPoMoCo.cpp
    ddiFourLoPoMoCo.cpp

    ddiMEI.cpp

    ddiSTG.cpp
    ddiSTGLowLevelIO.cpp
    ddiWinSTG.cpp
    ddiWinSTGLowLevelIO.cpp

    ddiDMM16AT.cpp
    ddiRMM1612.cpp

  ddiCompositeMRRobotController.cpp
    )

# all header files
SET(HEADER_FILES
    ddiCommandBase.h

    ddiCommand0.h
    ddiCommand1.h
    ddiCommand2.h

    ddiCommand0Base.h
    ddiCommand1Base.h
    ddiCommand2Base.h

    ddiCompositeCommand0.h
    ddiCompositeCommand1.h
    ddiCompositeCommand2.h

    ddiCompositeDevice.h
    ddiDeviceInterface.h
    ddiExport.h
    ddiNOPS.h
    ddiTypes.h
    ddiDynamicVectorDataObject.h
    ddiFixedSizeVectorDataObject.h
    ddiBasicTypeDataObject.h


    ddiATIDAQFT.h
    ddiATIDAQFTLowLevelIO.h
    ddiATIISAFT.h
    ddiATIISAFTLowLevelIO.h

    ddiLoPoMoCo.h
    ddiLoPoMoCoLowLevelIO.h
    ddiTwoLoPoMoCo.h
    ddiThreeLoPoMoCo.h
    ddiFourLoPoMoCo.h

    ddiMEI.h

    ddiSTG.h
    ddiSTGLowLevelIO.h
    ddiWinSTG.h
    ddiWinSTGLowLevelIO.h

    ddiDMM16AT.h
    ddiRMM1612.h

  ddiCompositeMRRobotController.h
    )


# specific options for this library

# make sure all options are set for all OS to OFF
SET(CISST_HAS_ATIDAQFT OFF CACHE BOOL "Build ATI DAQ F/T card extension")
MARK_AS_ADVANCED(FORCE CISST_HAS_ATIDAQFT)

SET(CISST_HAS_ATIISAFT OFF CACHE BOOL "Build ATI ISA F/T card extension")
MARK_AS_ADVANCED(FORCE CISST_HAS_ATIISAFT)

SET(CISST_HAS_MEI OFF CACHE BOOL "Build MEI card extension")
MARK_AS_ADVANCED(FORCE CISST_HAS_MEI)

SET(CISST_HAS_WINSTG OFF CACHE BOOL "Build ServoToGo (with Windows Driver) card extension")
MARK_AS_ADVANCED(FORCE CISST_HAS_WINSTG)

SET(CISST_HAS_LOPOMOCO OFF CACHE BOOL "Build LoPoMoCo card extension")
MARK_AS_ADVANCED(FORCE CISST_HAS_LOPOMOCO)

SET(CISST_HAS_STG OFF CACHE BOOL "Build ServoToGo card extension")
MARK_AS_ADVANCED(FORCE CISST_HAS_STG)

# hardware supported under Win32
IF(WIN32)

  # check for ATI DAQ
  OPTION(CISST_HAS_ATIDAQFT "" OFF)
  MARK_AS_ADVANCED(CLEAR CISST_HAS_ATIDAQFT)
  IF(CISST_HAS_ATIDAQFT)
    # find the NiDAQ libraries
    INCLUDE(${cisst_SOURCE_DIR}/libs/cmake/FindNIDAQ.cmake)
    IF(NIDAQ_INCLUDE_DIR)
      INCLUDE_DIRECTORIES(${NIDAQ_INCLUDE_DIR})
    ELSE(NIDAQ_INCLUDE_DIR)
      MESSAGE("Can't find the NiDAQ libraries")       
    ENDIF(NIDAQ_INCLUDE_DIR)
  ENDIF(CISST_HAS_ATIDAQFT)

  # check for WinSTG
  OPTION(CISST_HAS_WINSTG "" OFF)
  MARK_AS_ADVANCED(CLEAR CISST_HAS_WINSTG)

ENDIF(WIN32)

# check for MEI
OPTION(CISST_HAS_MEI "" OFF)
MARK_AS_ADVANCED(CLEAR CISST_HAS_MEI)
IF(CISST_HAS_MEI)
# find the MEI libraries
  INCLUDE(${cisst_SOURCE_DIR}/libs/cmake/FindMEI.cmake)
  IF(MEI_INCLUDE_DIR)
    INCLUDE_DIRECTORIES(${MEI_INCLUDE_DIR})
  ELSE(MEI_INCLUDE_DIR)
    MESSAGE("Can't find the MEI libraries")       
  ENDIF(MEI_INCLUDE_DIR)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMEI_LYNX -DMEI_LINUX")
ENDIF(CISST_HAS_MEI)

# hardware supported under Linux RTAI
IF(CISST_HAS_LINUX_RTAI)

  # check for STG
  OPTION(CISST_HAS_STG "" OFF)
  MARK_AS_ADVANCED(CLEAR CISST_HAS_STG)

  # check for LoPoMoCo
  OPTION(CISST_HAS_LOPOMOCO "" OFF)
  MARK_AS_ADVANCED(CLEAR CISST_HAS_LOPOMOCO)

ENDIF(CISST_HAS_LINUX_RTAI)


CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)

IF(CISST_HAS_MEI)
    TARGET_LINK_LIBRARIES(cisstDeviceInterface ${MEI_LIBRARY})
ENDIF(CISST_HAS_MEI)

#GSF
INCLUDE_DIRECTORIES(/pub/dscud5/)
TARGET_LINK_LIBRARIES(cisstDeviceInterface /pub/dscud5/libdscud5.a)

#
# $Log: CMakeLists.txt,v $
# Revision 1.18  2006/05/07 04:45:26  kapoor
# cisstDeviceInterface and IRE: Fixed printing of dataobjects through python.
#
# Revision 1.17  2006/05/05 19:45:12  kapoor
# cisstDeviceInterface CMakeLists: Link against MEI libraries if needed.
#
# Revision 1.16  2006/05/05 05:05:46  kapoor
# cisstDeviceInterface wrapping: Moved GainData to ddiTypes (to be split later),
# wrappers for basic types (double, int etc) to be passed through mailbox.
#
# Revision 1.15  2006/05/02 21:25:36  anton
# cisstDeviceInterface CMakeLists.txt: Updated and organized lists of files.
#
# Revision 1.14  2006/05/02 20:35:22  anton
# Data objects for ddi and rts: Moved data definition to the device interface
# level and renamed the two classes of "vectors".  Updated LoPoMoCo to verify
# compilation.
#
# Revision 1.13  2005/09/07 06:52:41  kapoor
# CMake: Minor modification for cisstDeviceInterface.
#
# Revision 1.12  2005/08/02 04:18:34  kapoor
# DeviceInterface: Fixed typo
#
# Revision 1.11  2005/07/30 06:15:02  kapoor
# DeviceInterface: (MEI) Added roboust cmake thingies for MEI.
#
# Revision 1.10  2005/06/21 15:31:48  anton
# cisstDeviceInterface CMakeLists.txt: Added dependency on cisstOSAbstraction.
#
# Revision 1.9  2005/06/17 22:45:25  anton
# cisst libraries: Added some CISST_EXPORT required to compile DLLs
#
# Revision 1.8  2005/02/28 22:35:04  anton
# cisstDeviceInterface:  Added ddiTwoLoPoMoCo.
#
# Revision 1.7  2005/02/03 21:48:37  anton
# cisstDeviceInterface: Renamed ddiTroisLoPoMoCo to ddiThreeLoPoMoCo
# and ddiQuartLoPoMoCo to ddiFourLoPoMoCo.
#
# Revision 1.6  2005/01/12 05:02:08  kapoor
# Added code for composite device for 3 and 4 LoPoMoCo boards. See Ticket #118
#
# Revision 1.5  2004/05/27 17:09:25  anton
# Incorporated code from Ankur for the ATI, MEI, STG and LoPoMoCo devices.
# The build process will have to be redone whenever will will add dynamic
# loading.
#
# Revision 1.4  2004/05/11 17:55:27  anton
# Switch back to ddiCompositeCommand from ddiCommandComposite.  There has been
# a minor communication problem between Ankur and I.  Sorry.
#
# Revision 1.3  2004/05/06 17:58:26  kapoor
# Renamed ddiCompositeCommand to ddiCommandComposte and ddiCompositeCommand2 to ddiCommand2Composite
#
# Revision 1.2  2004/05/04 20:45:45  anton
# Added code for composite devices
#
# Revision 1.1  2004/05/04 14:12:11  anton
# Imported code from Ankur
#
#
