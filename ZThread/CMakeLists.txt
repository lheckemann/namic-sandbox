project( ZThread )

cmake_minimum_required(VERSION 2.4)

#The following lines are required to use Dart
enable_testing()
include(Dart)

file ( GLOB Source src/*.cxx )

add_library ( ZThread ${Source} )
include_directories ( ${CMAKE_SOURCE_DIR}/include )

