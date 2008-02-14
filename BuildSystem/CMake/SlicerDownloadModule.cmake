INCLUDE(SlicerSetGetModule)

# ---------------------------------------------------------------------------
# SLICER_CREATE_DOWNLOAD_MODULE_TARGET: Create a download module target.
#
# This macro can be used to create a target to download a module.
#
# Arguments:
#   module_varname (string): variable name used to store the module values
#   target_varname (string): variable name used to store the target name
# 
# Example:
#   SLICER_CREATE_DOWNLOAD_MODULE_TARGET(TestModule target)
#
# See also:
#   SLICER_PARSE_MODULE_FILE
# ---------------------------------------------------------------------------

MACRO(SLICER_CREATE_DOWNLOAD_MODULE_TARGET module_varname target_varname)
  
  SLICER_GET_MODULE_SOURCE_REPOSITORY_TYPE(TestModule1 type)
  IF(type STREQUAL "svn")
    
  ENDIF(type STREQUAL "svn")

ENDMACRO(SLICER_CREATE_DOWNLOAD_MODULE_TARGET)
