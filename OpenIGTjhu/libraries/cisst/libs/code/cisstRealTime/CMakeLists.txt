#
# $Id: 
#
# CMakeLists for cisstRealTime
#

# set the library name
SET(LIBRARY cisstRealTime)

# set dependencies
SET(DEPENDENCIES
    cisstCommon
    cisstOSAbstraction
    cisstDeviceInterface)

# all source files
SET(SOURCE_FILES
    rtsStateDataTable.cpp
    rtsTask.cpp
    rtsTaskManager.cpp
    )

# all header files
SET(HEADER_FILES
    rtsExport.h
    rtsCommand0.h
    rtsCommand0Base.h
    rtsCommand1.h
    rtsCommand1Base.h
    rtsStateDataArray.h
    rtsStateDataArrayBase.h
    rtsStateDataTable.h
    rtsTask.h
    rtsTaskManager.h
    rtsTimeIndex.h
    )


CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)


#
# $Log: CMakeLists.txt,v $
# Revision 1.8  2006/05/04 05:46:45  kapoor
# Extended command0,1 to tasks. Appears to work. More testing needed.
#
# Revision 1.7  2006/05/02 20:35:22  anton
# Data objects for ddi and rts: Moved data definition to the device interface
# level and renamed the two classes of "vectors".  Updated LoPoMoCo to verify
# compilation.
#
# Revision 1.6  2005/09/06 02:08:15  anton
# CMake configuration: Created a main CMakeLists.txt for the libs, tests and
# examples.  A lot of code has been "factorized" in the ./CMakeLists.txt.
# Known problems: "doc" is not working yet. Dependencies on SWIG projects don't
# work for parallel builds. If a lib is selected and then unselected, the tests
# still appear ...
#
# Revision 1.5  2005/06/20 22:13:42  anton
# cisstRealtime: Added dependency on cisstDeviceInterface for DLLs
#
# Revision 1.4  2005/06/19 21:40:19  kapoor
# cisstRealTime: missing header file in CMakeList.
#
# Revision 1.3  2005/06/17 20:30:50  alamora
# cisstOSAbstraction and cisstRealTime: Added some required
# CISST_EXPORT to compile DLLs.
#
# Revision 1.2  2004/05/05 21:16:03  anton
# Added missing files for cisstRealTime from Ankur
# Corrected bug #28
#
# Revision 1.1  2004/05/04 14:12:11  anton
# Imported code from Ankur
#
#
