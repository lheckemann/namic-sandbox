cmake_minimum_required(VERSION 2.5)
include(SlicerSetGetModule)

# ---------------------------------------------------------------------------
# SLICER_CREATE_USE_MODULE_OPTION: Create an option to use a module.
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
#   SLICER_CREATE_USE_MODULE_OPTION(TestModule USE_TEST_MODULE)
#   IF(USE_TEST_MODULE)
#     ...
#   ENDIF(USE_TEST_MODULE)
#
# See also:
#   SLICER_GET_MODULE_VALUE
#   SLICER_SET_MODULE_VALUE
#   SLICER_GET_MODULE_SHORT_DESCRIPTION
# ---------------------------------------------------------------------------

function(SLICER_CREATE_USE_MODULE_OPTION module_varname option_name)

  # Unknown module? Bail.

  slicer_is_module_unknown(
    ${module_varname} unknown "Unable to create option!")
  if(unknown)
    return()
  endif(unknown)

  # Create the option

  slicer_get_module_short_description(${module_varname} short_desc)
  option(${option_name} "Use ${short_desc}." OFF)

endfunction(SLICER_CREATE_USE_MODULE_OPTION)
