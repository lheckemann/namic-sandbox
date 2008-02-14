INCLUDE(SlicerSetGetModule)

# ---------------------------------------------------------------------------
# SLICER_CREATE_USE_MODULE_OPTION: Create an option to use a module.
#
# This macro can be used to create a boolean OPTION variable to control if
# a module should be used or not.
#
# Arguments:
#   module_varname (string): variable name used to store the module keys/values
#   option_varname (string): variable name used to store the option name
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

  SLICER_GET_MODULE_VALUE(${module_varname} "Name" name)
  IF(name)
    SET(option_name "SLICER_USE_MODULE_${name}")
    SLICER_GET_MODULE_SHORT_DESCRIPTION(${module_varname} short_desc)
    OPTION(${option_name} "Use ${short_desc}." OFF)
    SET(${option_varname} ${option_name} PARENT_SCOPE)
  ENDIF(name)

ENDFUNCTION(SLICER_CREATE_USE_MODULE_OPTION)
