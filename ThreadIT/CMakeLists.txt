project ( ThreadIT )

cmake_minimum_required ( VERSION 2.6 )

find_package ( ITK )
if ( ITK_FOUND )
  include(${ITK_USE_FILE})
else ( ITK_FOUND )
  message ( FATAL_ERROR "Cannot build without ITK" )
endif ( ITK_FOUND )


subdirs ( ZThread )
# dependent libraries
link_libraries( ${ITK_LIBRARIES} ZThread )

include_directories ( ${CMAKE_SOURCE_DIR}/ZThread/include )
add_executable ( ThreadIT ThreadIT.cxx )

enable_testing()
include(Dart)
add_test(ThreadIT ThreadIT)
