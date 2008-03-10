cmake_minimum_required(VERSION 2.5)
include(SlicerSetGetModule)
find_package(Subversion)
find_package(CVS)

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

function(SLICER_CREATE_DOWNLOAD_MODULE_TARGET module_varname target_name dir)

  # Unknown module? Bail.

  set(err_msg "Unable to create download target!")
  slicer_is_module_unknown(${module_varname} unknown ${err_msg})
  if(unknown)
    return()
  endif(unknown)

  # Missing source location? Bye.

  slicer_get_module_value(${module_varname} "SourceLocation" source_loc)
  if(NOT source_loc)
    message(SEND_ERROR 
      "Unknown source location for module ${module_varname}. ${err_msg}")
    return()
  endif(NOT source_loc)

  slicer_get_module_source_repository_type(TestModule1 type)
  if(type STREQUAL "svn")
    # Missing subversion? Gone. Otherwise create a command to checkout
    # the repository, assume it is there if the .svn directory was found.
    if(NOT Subversion_FOUND)
      message(SEND_ERROR "Subversion was not found. ${err_msg}")
      return()
    endif(NOT Subversion_FOUND)
    add_custom_command(
      OUTPUT "${dir}/.svn"
      COMMAND "${Subversion_SVN_EXECUTABLE}"
      ARGS "co" "${source_loc}" "${dir}"
      )
    add_custom_target(${target_name} DEPENDS "${dir}/.svn")
  endif(type STREQUAL "svn")

endfunction(SLICER_CREATE_DOWNLOAD_MODULE_TARGET)
