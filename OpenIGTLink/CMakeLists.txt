PROJECT( OpenIGTLink )

ENABLE_TESTING()
INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)


#
#  There should be only one INCLUDE_DIRECTORIES command 
#  in the entire tree in order to enforce a particular
#  order for the search.
#
INCLUDE_DIRECTORIES(
  ${OpenIGTLink_SOURCE_DIR}/Source 
  ${OpenIGTLink_SOURCE_DIR}/Source/igtlutil 
  ${OpenIGTLink_SOURCE_DIR}/Tools 
  )

ADD_SUBDIRECTORY( Source    )
ADD_SUBDIRECTORY( Testing   )
ADD_SUBDIRECTORY( Tools     )
ADD_SUBDIRECTORY( Documents )


