PROJECT( OpenIGTLink )

ENABLE_TESTING()

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

SUBDIRS(
  Source
  Testing
  Tools
  Documents
  )


