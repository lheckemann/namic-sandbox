# ---------------------------------------------------------------------------
# SLICER_SET_MODULE_VALUE: Set a module value.
#
# This macro can be used to set a module value without worrying too much
# about the underlying data structure. 
# Values are stored as key/value pairs at the moment.
#
# Arguments:
#   module_varname (string): variable name used to store the module values
#   key (string): key
#   value (string): value
# 
# Example:
#   SLICER_SET_MODULE_VALUE(TestModule "Author" "John Doe")
#
# See also:
#   SLICER_GET_MODULE_VALUE
#   SLICER_UNSET_MODULE_VALUE
# ---------------------------------------------------------------------------

MACRO(SLICER_SET_MODULE_VALUE module_varname key value)
  
  SET(${module_varname}_${key} ${value})

ENDMACRO(SLICER_SET_MODULE_VALUE)

# ---------------------------------------------------------------------------
# SLICER_GET_MODULE_VALUE: Get a module value.
#
# This macro can be used to retrieve a module value without worrying too much
# about the underlying data structure. 
# Values are stored as key/value pairs at the moment.
#
# Arguments:
#   module_varname (string): variable name used to store the module values
#   key (string): key
#   value_varname (string): variable name used to store the value
# 
# Example:
#   SLICER_GET_MODULE_VALUE(TestModule "Author" authors)
#   MESSAGE("Author(s): ${author}")
#
# See also:
#   SLICER_SET_MODULE_VALUE
#   SLICER_UNSET_MODULE_VALUE
# ---------------------------------------------------------------------------

MACRO(SLICER_GET_MODULE_VALUE module_varname key value_varname)

  IF(DEFINED ${module_varname}_${key})
    SET(${value_varname} ${${module_varname}_${key}})
  ELSE(DEFINED ${module_varname}_${key})
    SET(${value_varname})
  ENDIF(DEFINED ${module_varname}_${key})

ENDMACRO(SLICER_GET_MODULE_VALUE)

# ---------------------------------------------------------------------------
# SLICER_UNSET_MODULE_VALUE: unset a module value.
#
# This macro can be used to unset a module value without worrying too much
# about the underlying data structure. 
# Values are stored as key/value pairs at the moment.
#
# Arguments:
#   module_varname (string): variable name used to store the module values
#   key (string): key
# 
# Example:
#   SLICER_UNSET_MODULE_VALUE(TestModule "Author")
#
# See also:
#   SLICER_GET_MODULE_VALUE
#   SLICER_SET_MODULE_VALUE
# ---------------------------------------------------------------------------

MACRO(SLICER_UNSET_MODULE_VALUE module_varname key)
  
  SET(${module_varname}_${key})

ENDMACRO(SLICER_UNSET_MODULE_VALUE)
