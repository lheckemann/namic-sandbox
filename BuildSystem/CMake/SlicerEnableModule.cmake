cmake_minimum_required(VERSION 2.5)
include(SlicerSetGetModule)

# ---------------------------------------------------------------------------
# slicer_create_use_module_option: create an option to use a module.
#
# This function can be used to create a boolean OPTION variable to control if
# a module should be used or not.
#
# Arguments:
# in:
#   module_varname (string): variable name used to store the module keys/values
#   option_name (string): name to use to for the option
# 
# Example:
#   slicer_parse_module_file("C:/foo/TestModule/TestModule.xml" TestModule)
#   slicer_create_use_module_option(TestModule USE_TEST_MODULE)
#   if(USE_TEST_MODULE)
#     ...
#   endif(USE_TEST_MODULE)
#
# See also:
#   slicer_get_module_value
#   slicer_set_module_value
#   slicer_get_module_short_description
# ---------------------------------------------------------------------------

function(slicer_create_use_module_option module_varname option_name)

  # Unknown module? Bail.

  slicer_is_module_unknown(
    ${module_varname} unknown "Unable to create option!")
  if(unknown)
    return()
  endif(unknown)

  # Create the option

  slicer_get_module_short_description(${module_varname} short_desc)
  option(${option_name} "Use ${short_desc}." OFF)

endfunction(slicer_create_use_module_option)
