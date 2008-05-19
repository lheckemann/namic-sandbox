#
# $Id: 
#
# CMakeLists for cisstTracker
#

# set the library name
SET(LIBRARY cisstTracker)

# set dependencies
SET(DEPENDENCIES
    cisstCommon
    cisstOSAbstraction
    cisstRealTime
    cisstVector)

# all source files
SET(SOURCE_FILES
    )

# all header files
SET(HEADER_FILES
    trkTask.h
    )


CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)


#
# $Log: CMakeLists.txt,v $
# Revision 1.2  2005/09/06 02:08:15  anton
# CMake configuration: Created a main CMakeLists.txt for the libs, tests and
# examples.  A lot of code has been "factorized" in the ./CMakeLists.txt.
# Known problems: "doc" is not working yet. Dependencies on SWIG projects don't
# work for parallel builds. If a lib is selected and then unselected, the tests
# still appear ...
#
# Revision 1.1  2004/05/04 14:12:11  anton
# Imported code from Ankur
#
#
