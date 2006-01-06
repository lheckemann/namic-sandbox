PROJECT ( CLRegistration )

FIND_PACKAGE ( ITK )
IF ( ITK_FOUND )
  INCLUDE(${ITK_USE_FILE})
ELSE ( ITK_FOUND )
  MESSAGE ( FATAL_ERROR "Cannot build without ITK" )
ENDIF ( ITK_FOUND )

INCLUDE_DIRECTORIES (
                    "${CLRegistration_SOURCE_DIR}/tclap/include"
                    "${CLRegistration_SOURCE_DIR}/cli4cpp-0.2/include" )

SET ( SOURCE CLRegistration.cxx 
cli4cpp-0.2/src/BasicParser.cpp
cli4cpp-0.2/src/CommandLine.cpp
cli4cpp-0.2/src/HelpFormatter.cpp
cli4cpp-0.2/src/Option.cpp
cli4cpp-0.2/src/Options.cpp
)

ADD_EXECUTABLE ( CLRegistration ${SOURCE})
TARGET_LINK_LIBRARIES ( CLRegistration ITKIO ITKCommon ITKBasicFilters ITKAlgorithms ITKNumerics ITKStatistics)

