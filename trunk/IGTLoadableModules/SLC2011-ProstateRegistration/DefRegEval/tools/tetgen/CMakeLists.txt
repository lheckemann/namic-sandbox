cmake_minimum_required (VERSION 2.6)
project (tetgen)

# Set Windows-specific command line switches
IF(WIN32 AND NOT CYGWIN)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_CRT_SECURE_NO_WARNINGS")
ENDIF(WIN32 AND NOT CYGWIN)

add_executable (tetgen
  tetgen.cxx
  predicates.cxx
)
