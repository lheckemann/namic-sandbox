PROJECT ( CLRegistration )

FIND_PACKAGE ( ITK )
IF ( ITK_FOUND )
  INCLUDE(${ITK_USE_FILE})
ELSE ( ITK_FOUND )
  MESSAGE ( FATAL_ERROR "Cannot build without ITK" )
ENDIF ( ITK_FOUND )

UTILITY_SOURCE(GENERATECLP_EXE GenerateCLP ./ GenerateCLP.cxx)
# create the .clp files
# usage: GENERATE_CLP(foo_SRCS ${XML_FILES})
MACRO(GENERATECLP SOURCES XML)
  # For each XML file
  FOREACH(FILE ${XML})
    # what is the filename without the extension
    GET_FILENAME_COMPONENT(TMP_FILENAME ${FILE} NAME_WE)
        
    # the input file might be full path so handle that
    GET_FILENAME_COMPONENT(TMP_FILEPATH ${FILE} PATH)

    # compute the input filename
    IF (TMP_FILEPATH)
      SET(TMP_INPUT ${TMP_FILEPATH}/${TMP_FILENAME}.xml) 
    ELSE (TMP_FILEPATH)
      SET(TMP_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/${TMP_FILENAME}.xml)
    ENDIF (TMP_FILEPATH)

    # add custom command to output
    ADD_CUSTOM_COMMAND(
      OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${TMP_FILENAME}.clp
      DEPENDS GenerateCLP ${TMP_INPUT}
      COMMAND ${GENERATECLP_EXE}
        ${TMP_INPUT} ${CMAKE_CURRENT_SOURCE_DIR}/${TMP_FILENAME}.clp
      )
    # mark the .clp file as a header file
    SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/${TMP_FILENAME}.clp PROPERTIES HEADER_FILE_ONLY TRUE)
    SET_SOURCE_FILES_PROPERTIES(${TMP_FILENAME}.cxx PROPERTIES OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${TMP_FILENAME}.clp)


    SET(${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/${TMP_FILENAME}.clp ${${SOURCES}}) 
    ENDFOREACH(FILE)
ENDMACRO(GENERATECLP)

INCLUDE_DIRECTORIES (
                    "${CLRegistration_SOURCE_DIR}/tclap/include"
                    "${CLRegistration_SOURCE_DIR}/cli4cpp-0.3/include" )

SET ( SOURCE CLRegistration.cxx 
cli4cpp-0.3/src/BasicParser.cpp
cli4cpp-0.3/src/Parser.cpp
cli4cpp-0.3/src/CommandLine.cpp
cli4cpp-0.3/src/HelpFormatter.cpp
cli4cpp-0.3/src/JSONHelpFormatter.cpp
cli4cpp-0.3/src/Option.cpp
cli4cpp-0.3/src/Options.cpp
)

SET (GENERATECLP_SOURCE GenerateCLP.cxx)
ADD_EXECUTABLE ( GenerateCLP ${GENERATECLP_SOURCE})
TARGET_LINK_LIBRARIES ( GenerateCLP ITKEXPAT)

SET ( CLPEXAMPLE1_SOURCE CLPExample1.cxx )
GENERATECLP(CLPEXAMPLE1_SOURCE CLPExample1.xml)
ADD_EXECUTABLE ( CLPExample1 ${CLPEXAMPLE1_SOURCE})
TARGET_LINK_LIBRARIES ( CLPExample1 ITKEXPAT)
