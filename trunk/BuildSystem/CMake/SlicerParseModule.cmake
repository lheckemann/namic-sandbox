INCLUDE(SlicerSetGetModule)

# ---------------------------------------------------------------------------
# SLICER_PARSE_MODULE: Parse a module.
#
# This macro parses a module and creates the corresponding key/value pairs.
#
# Arguments:
#   module_contents (string): contents of the module
#   module_varname (string): variable name used to store the module values
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

MACRO(SLICER_PARSE_MODULE module_contents module_varname)
  
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

ENDMACRO(SLICER_PARSE_MODULE)

# ---------------------------------------------------------------------------
# SLICER_PARSE_MODULE_FILE: Load and parse a module.
#
# This macro loads a module into a variable and parse its contents by calling
# the SLICER_PARSE_MODULE macro.
#
# Arguments:
#   module_filename (filename): filename for the module
#   module_varname (string): variable name used to store the module values
# 
# Example:
#   SLICER_PARSE_MODULE_FILE("TestModule.xml" TestModule)
#   SLICER_GET_MODULE_VALUE(TestModule "Name" name)
#   MESSAGE("Module name: ${name}")
#
# See also:
#   SLICER_PARSE_MODULE
# ---------------------------------------------------------------------------

MACRO(SLICER_PARSE_MODULE_FILE module_filename module_varname)
  IF(EXISTS "${module_filename}")
    FILE(READ "${module_filename}" module_contents)
    SLICER_PARSE_MODULE("${module_contents}" ${module_varname})
  ELSE(EXISTS "${module_filename}")
    MESSAGE("Unable to load and parse module ${module_filename}!")
  ENDIF(EXISTS "${module_filename}")
ENDMACRO(SLICER_PARSE_MODULE_FILE)
