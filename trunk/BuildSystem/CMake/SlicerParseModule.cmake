cmake_minimum_required(VERSION 2.5)
include(SlicerSetGetModule)

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

function(SLICER_PARSE_MODULE module_contents module_varname)
  
  # The XML elements to parse
  # This doesn't take into account any attributes at the moment

  set(elems 
    Acknowledgement
    Author
    CVSBranch
    CVSModule
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

  foreach(elem ${elems})
    slicer_unset_module_value(${module_varname} ${elem})
    set(regexp "<${elem}>([^<]*)</${elem}>")
    string(REGEX MATCHALL "${regexp}" matches "${module_contents}")
    foreach(match ${matches})
      string(REGEX MATCH "${regexp}" value "${match}")
      if(CMAKE_MATCH_1)
        slicer_get_module_value(${module_varname} ${elem} var)
        set(var ${var} "${CMAKE_MATCH_1}")
        slicer_set_module_value(${module_varname} ${elem} "${var}")
      endif(CMAKE_MATCH_1)
    endforeach(match)
  endforeach(elem)

endfunction(SLICER_PARSE_MODULE)

# ---------------------------------------------------------------------------
# SLICER_PARSE_MODULE_FILE: Load and parse a module.
#
# This function loads a module into a variable and parse its contents by calling
# the SLICER_PARSE_MODULE function.
# Note: the file name can be a directory, say /home/foo/module1, in that case
# this function will try to read /home/foo/module1/module1.xml
#
# Arguments:
# in:
#   module_filename (filename): full path (filename) to the module
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

function(SLICER_PARSE_MODULE_FILE module_filename module_varname)

  # If filename is a directory, try to look for a XML file inside it with
  # the same name as the directory...

  if(IS_DIRECTORY "${module_filename}")
    get_filename_component(abs "${module_filename}" ABSOLUTE)
    get_filename_component(dirname "${abs}" NAME)
    get_filename_component(dirpath "${abs}" PATH)
    set(module_filename "${dirpath}/${dirname}/${dirname}.xml")
  endif(IS_DIRECTORY "${module_filename}")
    
  if(NOT EXISTS "${module_filename}")
    message(SEND_ERROR "Unable to load and parse module ${module_filename}!")
    return()
  endif(NOT EXISTS "${module_filename}")

  # Parse it

  file(READ "${module_filename}" module_contents)
  slicer_parse_module("${module_contents}" ${module_varname})

endfunction(SLICER_PARSE_MODULE_FILE)
