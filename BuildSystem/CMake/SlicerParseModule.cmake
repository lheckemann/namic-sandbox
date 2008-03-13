cmake_minimum_required(VERSION 2.5)
include(SlicerSetGetModule)

# ---------------------------------------------------------------------------
# slicer_parse_module: Parse a module.
#
# This function parses a module and creates the corresponding key/value pairs.
#
# Note: the list of modules that have been parsed so far can be retrieved
# using slicer_get_parsed_modules_list
#
# Arguments:
# in:
#   module_contents (string): contents of the module
# in/out:
#   module_varname (string): variable name to use to store the module values
# 
# Example:
#   SET(module_contents "<Name>TestModule</Name><Group>Segmentation</Group>")
#   slicer_parse_module("${module_contents}" TestModule)
#   slicer_get_module_value(TestModule Name name)
#   message("Module name: ${name}")
#
# See also:
#   slicer_parse_module_file
#   slicer_get_parsed_modules_list
# ---------------------------------------------------------------------------

function(slicer_parse_module module_contents module_varname)
  
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
        slicer_set_module_value(${module_varname} ${elem} ${var})
      endif(CMAKE_MATCH_1)
    endforeach(match)
  endforeach(elem)

  # Keep track of the modules parsed so far

  slicer_get_module_value(__Meta__ ParsedModules parsed_modules)
  set(parsed_modules ${parsed_modules} ${module_varname})
  list(REMOVE_DUPLICATES parsed_modules)
  slicer_set_module_value(__Meta__ ParsedModules ${parsed_modules})

endfunction(slicer_parse_module)

# ---------------------------------------------------------------------------
# slicer_parse_module_file: Load and parse a module.
#
# This function loads a module into a variable and parse its contents by calling
# the slicer_parse_module function.
# The name of the file (without extension) will be used if the variable name
# to use to store the module values is ommitted.
#
# Note: the file name can be a directory, say /home/foo/module1, in that case
# this function will try to read /home/foo/module1/module1.xml
#
# Note: the list of modules that have been parsed so far can be retrieved
# using slicer_get_parsed_modules_list
#
# Arguments:
# in:
#   module_filename (filename): full path (filename) to the module
# optional in/out:
#   module_varname (string): variable name to use to store the module values
# 
# Example:
#   slicer_parse_module_file("C:/foo/TestModule/TestModule.xml" TestModule)
#   slicer_get_module_value(TestModule Name name)
#   message("Module name: ${name}")
#
#   slicer_parse_module_file("C:/foo/TestModule2/")
#   slicer_get_module_value(TestModule2 Name name)
#   message("Module2 name: ${name}")
#
# See also:
#   slicer_parse_module
#   slicer_get_parsed_modules_list
# ---------------------------------------------------------------------------

function(slicer_parse_module_file module_filename)

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

  # Module varname variable (use the filename if not found)

  if(ARG1)
    set(module_varname ${ARGV1})
  else(ARG1)
    get_filename_component(module_varname "${module_filename}" NAME_WE)
  endif(ARG1)

  # Parse it

  file(READ "${module_filename}" module_contents)
  slicer_parse_module("${module_contents}" ${module_varname})

endfunction(slicer_parse_module_file)

# ---------------------------------------------------------------------------
# slicer_get_parsed_modules_list: Parse a module.
#
# This function can be used to retrieve the list of modules that have
# been parsed so far.
#
# Arguments:
# out:
#   list_varname (string): variable name to use to store the modules list
# 
# Example:
#   slicer_parse_module_file("TestModule.xml" TestModule)
#   slicer_parse_module_file("TestModule2.xml" TestModule2)
#   slicer_get_parsed_modules_list(modules_list)
#
# See also:
#   slicer_parse_module
#   slicer_parse_module_file
# ---------------------------------------------------------------------------

function(slicer_get_parsed_modules_list list_varname)

  slicer_get_module_value(__Meta__ ParsedModules value)
  set(${list_varname} ${value} PARENT_SCOPE)
  
endfunction(slicer_get_parsed_modules_list)

