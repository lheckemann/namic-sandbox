INCLUDE(SlicerSetGetModule)

# ---------------------------------------------------------------------------
# SLICER_GET_MODULE_SHORT_DESCRIPTION: Get a module short description.
#
# This macro uses the module variables to create a short module description.
#
# Arguments:
#   module_varname (string): variable name used to store the module keys/values
#   desc_varname (string):   variable name used to store the short description
# 
# See also:
#   SLICER_GET_MODULE_VALUE
#   SLICER_SET_MODULE_VALUE
# ---------------------------------------------------------------------------

FUNCTION(SLICER_GET_MODULE_SHORT_DESCRIPTION module_varname desc_varname)

  SLICER_GET_MODULE_VALUE(${module_varname} "Name" name)
  IF(name)
    SET(short_desc "${name}")
  ELSE(name)
    SET(short_desc "<Unknown>")
  ENDIF(name)

  SLICER_GET_MODULE_VALUE(${module_varname} "Version" version)
  IF(version)
    SET(short_desc "${short_desc} ${version}")
  ENDIF(version)

  SLICER_GET_MODULE_VALUE(${module_varname} "Author" authors)
  IF(authors)
    SET(short_desc "${short_desc} (${authors})")
  ENDIF(authors)

  SET(short_desc "${short_desc}.")

  SLICER_GET_MODULE_VALUE(${module_varname} "Description" desc)
  IF(desc)
    SET(short_desc "${short_desc} ${desc}")
  ENDIF(desc)

  SET(${desc_varname} ${short_desc} PARENT_SCOPE)

ENDFUNCTION(SLICER_GET_MODULE_SHORT_DESCRIPTION)

# ---------------------------------------------------------------------------
# SLICER_CREATE_MODULE_OPTION: Create an option to use a module.
#
# This macro can be used to create a boolean OPTION variable to control if
# a module should be used or not.
#
# Arguments:
#   module_varname (string): variable name used to store the module keys/values
#   option_varname (string): variable name used to store the option name
# 
# See also:
#   SLICER_GET_MODULE_VALUE
#   SLICER_SET_MODULE_VALUE
# ---------------------------------------------------------------------------

FUNCTION(SLICER_CREATE_MODULE_OPTION module_varname option_varname)

  SLICER_GET_MODULE_VALUE(${module_varname} "Name" name)
  IF(name)
    SET(option_name "SLICER_USE_MODULE_${name}")
    SLICER_GET_MODULE_SHORT_DESCRIPTION(${module_varname} short_desc)
    OPTION(${option_name} "Use ${short_desc}." OFF)
    SET(${option_varname} ${option_name} PARENT_SCOPE)
  ENDIF(name)

ENDFUNCTION(SLICER_CREATE_MODULE_OPTION)

