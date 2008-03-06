CMAKE_MINIMUM_REQUIRED(VERSION 2.5)
INCLUDE(SlicerSetGetModule)

# ---------------------------------------------------------------------------
# SLICER_CREATE_USE_MODULE_OPTION: Create an option to use a module.
#
# This function can be used to create a boolean OPTION variable to control if
# a module should be used or not.
#
# Arguments:
# in:
#   module_varname (string): variable name used to store the module keys/values
# out:
#   option_varname (string): variable name to use to store the option name
# 
# Example:
#   SLICER_CREATE_USE_MODULE_OPTION(TestModule use_module)
#   IF(use_module)
#     ...
#   ENDIF(use_module)
#
# See also:
#   SLICER_GET_MODULE_VALUE
#   SLICER_SET_MODULE_VALUE
#   SLICER_GET_MODULE_SHORT_DESCRIPTION
# ---------------------------------------------------------------------------

FUNCTION(SLICER_CREATE_USE_MODULE_OPTION module_varname option_varname)

  # Unknown module? Bail.

  SLICER_IS_MODULE_UNKNOWN(
    ${module_varname} unknown "Unable to create option!")
  IF(unknown)
    RETURN()
  ENDIF(unknown)

  # Create the option

  SET(option_name "SLICER_USE_MODULE_${name}")
  SLICER_GET_MODULE_SHORT_DESCRIPTION(${module_varname} short_desc)
  OPTION(${option_name} "Use ${short_desc}." OFF)
  SET(${option_varname} ${option_name} PARENT_SCOPE)

ENDFUNCTION(SLICER_CREATE_USE_MODULE_OPTION)
