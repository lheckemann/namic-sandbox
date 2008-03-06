CMAKE_MINIMUM_REQUIRED(VERSION 2.5)
INCLUDE(SlicerSetGetModule)

# ---------------------------------------------------------------------------
# SLICER_PARSE_MODULE: Parse a module.
#
# This function parses a module and creates the corresponding key/value pairs.
#
# Arguments:
# in:
#   module_contents (string): contents of the module
# in/out:
#   module_varname (string): variable name to use to store the module values
# 
# Example:
#   SET(module_contents "<Name>TestModule</Name><Group>Segmentation</Group>")
#   SLICER_PARSE_MODULE("${module_contents}" TestModule)
#   SLICER_GET_MODULE_VALUE(TestModule "Name" name)
#   MESSAGE("Module name: ${name}")
#
# See also:
#   SLICER_PARSE_MODULE_FILE
# ---------------------------------------------------------------------------

FUNCTION(SLICER_PARSE_MODULE module_contents module_varname)
  
  # The XML elements to parse
  # This doesn't take into account any attributes at the moment

  SET(elems 
    Acknowledgement
    Author
    Dependency
    Description 
    Group 
    HomePage 
    Icon 
    Name 
    SourceLocation 
    Version 
    )

  # For each element:
  # - make sure the variable is unset (say TestModule_Author)
  # - search for all matches (i.e. <Author>Bar1</Author>;<Author>Bar2</Author)
  # - loop over each match:
  #   * extract the value itself (i.e. Bar1, then Bar2)
  #   * insert the value at the end of the list for that element variable 

  FOREACH(elem ${elems})
    SLICER_UNSET_MODULE_VALUE(${module_varname} ${elem})
    SET(regexp "<${elem}>([^<]*)</${elem}>")
    STRING(REGEX MATCHALL "${regexp}" matches "${module_contents}")
    FOREACH(match ${matches})
      STRING(REGEX MATCH "${regexp}" value "${match}")
      IF(CMAKE_MATCH_1)
        SLICER_GET_MODULE_VALUE(${module_varname} ${elem} var)
        SET(var ${var} "${CMAKE_MATCH_1}")
        SLICER_SET_MODULE_VALUE(${module_varname} ${elem} "${var}")
      ENDIF(CMAKE_MATCH_1)
    ENDFOREACH(match)
  ENDFOREACH(elem)

ENDFUNCTION(SLICER_PARSE_MODULE)

# ---------------------------------------------------------------------------
# SLICER_PARSE_MODULE_FILE: Load and parse a module.
#
# This function loads a module into a variable and parse its contents by calling
# the SLICER_PARSE_MODULE function.
#
# Arguments:
# in:
#   module_filename (filename): filename for the module
# in/out:
#   module_varname (string): variable name to use to store the module values
# 
# Example:
#   SLICER_PARSE_MODULE_FILE("TestModule.xml" TestModule)
#   SLICER_GET_MODULE_VALUE(TestModule "Name" name)
#   MESSAGE("Module name: ${name}")
#
# See also:
#   SLICER_PARSE_MODULE
# ---------------------------------------------------------------------------

FUNCTION(SLICER_PARSE_MODULE_FILE module_filename module_varname)

  IF(NOT EXISTS "${module_filename}")
    MESSAGE(SEND_ERROR "Unable to load and parse module ${module_filename}!")
    RETURN()
  ENDIF(NOT EXISTS "${module_filename}")

  FILE(READ "${module_filename}" module_contents)
  SLICER_PARSE_MODULE("${module_contents}" ${module_varname})

ENDFUNCTION(SLICER_PARSE_MODULE_FILE)
