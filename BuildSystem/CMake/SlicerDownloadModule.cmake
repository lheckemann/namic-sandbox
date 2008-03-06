CMAKE_MINIMUM_REQUIRED(VERSION 2.5)
INCLUDE(SlicerSetGetModule)
INCLUDE(FindSubversion)

# ---------------------------------------------------------------------------
# SLICER_CREATE_DOWNLOAD_MODULE_TARGET: Create a download module target.
#
# This function can be used to create a target to download a module.
#
# Arguments:
# in:
#   module_varname (string): variable name used to store the module values
#   target_name (string): name of the target
#   dir (string): where the module should be downloaded
# 
# Example:
#   SLICER_CREATE_DOWNLOAD_MODULE_TARGET(TestModule target)
#
# See also:
#   SLICER_PARSE_MODULE_FILE
# ---------------------------------------------------------------------------

FUNCTION(SLICER_CREATE_DOWNLOAD_MODULE_TARGET module_varname target_name dir)

  # Unknown module? Bail.

  SET(err_msg "Unable to create download target!")
  SLICER_IS_MODULE_UNKNOWN(${module_varname} unknown ${err_msg})
  IF(unknown)
    RETURN()
  ENDIF(unknown)

  # Missing source location? Bye.

  SLICER_GET_MODULE_VALUE(${module_varname} "SourceLocation" source_loc)
  IF(NOT source_loc)
    MESSAGE(SEND_ERROR 
      "Unknown source location for module ${module_varname}. ${err_msg}")
    RETURN()
  ENDIF(NOT source_loc)

  SLICER_GET_MODULE_SOURCE_REPOSITORY_TYPE(TestModule1 type)
  IF(type STREQUAL "svn")
    # Missing subversion? Gone. Otherwise create a command to checkout
    # the repository, assume it is there if the .svn directory was found.
    IF(NOT Subversion_FOUND)
      MESSAGE(SEND_ERROR "Subversion was not found. ${err_msg}")
      RETURN()
    ENDIF(NOT Subversion_FOUND)
    ADD_CUSTOM_COMMAND(
      OUTPUT "${dir}/.svn"
      COMMAND "${Subversion_SVN_EXECUTABLE}"
      ARGS "co" "${source_loc}" "${dir}"
      )
    ADD_CUSTOM_TARGET(${target_name} DEPENDS "${dir}/.svn")
  ENDIF(type STREQUAL "svn")

ENDFUNCTION(SLICER_CREATE_DOWNLOAD_MODULE_TARGET)
