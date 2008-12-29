project( ImageNetworkReader )

cmake_minimum_required(VERSION 2.4)

#include some macros from another file...
include(${CMAKE_SOURCE_DIR}/IJMacros.txt)

#The following lines are required to use Dart
enable_testing()
include(Dart)

#Declare any external dependencies that your project may have here.
#examples include: ITK, VTK, JPEG, PNG, OpenGL, ZLIB, Perl, Java
#If you're not sure what name to use, look in the Modules directory of your
#cmake install and check that a file named Find(Package).cmake exists
SET(Required_Packages ITK)

#this foreach loads all of the packages that you specified as required.
#It shouldn't need to be modified.
foreach(Package ${Required_Packages})
  LOADPACKAGE(${Package})
endforeach(Package)

set(LIBRARY_OUTPUT_PATH "${ImageNetworkReader_BINARY_DIR}" CACHE PATH 
      "Single output directory for building all libraries.")
set(EXECUTABLE_OUTPUT_PATH ${ImageNetworkReader_BINARY_DIR} CACHE PATH 
      "Single output directory for building all executables.")
Subdirs ( curl )
Add_Executable(ImageNetworkReaderTest ImageNetworkReaderTest.cxx )
Target_Link_Libraries(ImageNetworkReaderTest ${ITK_LIBRARIES} curl) 
INCLUDE_DIRECTORIES(${ImageNetworkReader_SOURCE_DIR}/curl/curl)

ADD_DEFINITIONS ( -DTEST_CURL_SSL )
# Test it
Add_Test(ImageNetworkReaderTest ImageNetworkReaderTest http://www.na-mic.org/Wiki/images/f/f6/Test.nhdr)

