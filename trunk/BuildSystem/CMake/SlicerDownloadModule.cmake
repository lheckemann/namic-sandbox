cmake_minimum_required(VERSION 2.5)
include(SlicerSetGetModule)
find_package(Subversion)
find_package(CVS)

# ---------------------------------------------------------------------------
# SLICER_CREATE_DOWNLOAD_MODULE_TARGET: Create a download module target.
#
# This function can be used to create a target to download a module
# repository (CVS or SVN).
# Note: you probably would want to use SLICER_CREATE_UPDATE_MODULE_TARGET
# which will update the repository, and download it if it doesn't exist.
#
# Arguments:
# in:
#   module_varname (string): variable name used to store the module values
#   target_name (string): name of the target
#   dir (string): where the module should be downloaded
# 
# Example:
#   SLICER_CREATE_DOWNLOAD_MODULE_TARGET(TestModule testmodule_download)
#
# See also:
#   SLICER_CREATE_UPDATE_MODULE_TARGET
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

  slicer_get_module_source_repository_type(${module_varname} type)

  # If SVN create a command to checkout the repository, assume it is
  # there if the .svn/ sub-directory can be found.

  if(type STREQUAL "svn")

    if(NOT Subversion_FOUND)
      message(SEND_ERROR "Subversion (svn) was not found. ${err_msg}")
      return()
    endif(NOT Subversion_FOUND)
    
    set(args "checkout" "${source_loc}" "${dir}")

    add_custom_command(
      OUTPUT "${dir}/.svn/entries"
      COMMAND "${Subversion_SVN_EXECUTABLE}"
      ARGS ${args}
      )
    add_custom_target(${target_name} DEPENDS "${dir}/.svn/entries")

  # If CVS create a command to checkout the repository, assume it is
  # there if CVS/ sub-directory can be found.
  # Use the CVSModule parameter to get the module to checkout
  # Use the optional CVSBranch parameter to get the branch to checkout

  elseif(type STREQUAL "cvs")

    if(NOT CVS_FOUND)
      message(SEND_ERROR "CVS was not found. ${err_msg}")
      return()
    endif(NOT CVS_FOUND)

    slicer_get_module_value(${module_varname} "CVSModule" cvs_module)
    if(NOT cvs_module)
      message(SEND_ERROR 
        "Unknown CVS module for module ${module_varname}. ${err_msg}")
      return()
    endif(NOT cvs_module)

    if(NOT EXISTS "${dir}")
      file(MAKE_DIRECTORY "${dir}")
    endif(NOT EXISTS "${dir}")
      
    set(args "-d" "${source_loc}" "checkout" "-d" "${dir}")

    slicer_get_module_value(${module_varname} "CVSBranch" cvs_branch)
    if(cvs_branch)
      set(args ${args} "-r" "${cvs_branch}")
    endif(cvs_branch)

    set(args ${args} "${cvs_module}")

    add_custom_command(
      OUTPUT "${dir}/CVS/Root"
      COMMAND "${CVS_EXECUTABLE}"
      ARGS ${args}
      )
    add_custom_target(${target_name} DEPENDS "${dir}/CVS/Root")
  
  # Don't know the repository

  else(type STREQUAL "svn")

      message(SEND_ERROR "Unknown repository type. ${err_msg}")
      return()

  endif(type STREQUAL "svn")

endfunction(SLICER_CREATE_DOWNLOAD_MODULE_TARGET)

# ---------------------------------------------------------------------------
# SLICER_CREATE_UPDATE_MODULE_TARGET: Create an update module target.
#
# This function can be used to create a target to update a module 
# repository (CVS or SVN).
# Note: this function does not automatically create a download target.
#
# Arguments:
# in:
#   module_varname (string): variable name used to store the module values
#   target_name (string): name of the target
#   dir (string): where the module has been downloaded/checked out
# 
# Example:
#   SLICER_CREATE_UPDATE_MODULE_TARGET(TestModule testmodule_update)
#
# See also:
#   SLICER_CREATE_DOWNLOAD_MODULE_TARGET
# ---------------------------------------------------------------------------

function(SLICER_CREATE_UPDATE_MODULE_TARGET module_varname target_name dir)

  # Unknown module? Bail.

  set(err_msg "Unable to create update target!")
  slicer_is_module_unknown(${module_varname} unknown ${err_msg})
  if(unknown)
    return()
  endif(unknown)

  slicer_get_module_source_repository_type(${module_varname} type)

  # If SVN create a command to update the repository.

  if(type STREQUAL "svn")

    if(NOT Subversion_FOUND)
      message(SEND_ERROR "Subversion (svn) was not found. ${err_msg}")
      return()
    endif(NOT Subversion_FOUND)
    
    set(args "update" "--non-interactive" "${dir}")

    add_custom_target(${target_name}
      "${Subversion_SVN_EXECUTABLE}" ${args}
      )

  # If CVS create a command to update the repository.

  elseif(type STREQUAL "cvs")

    if(NOT CVS_FOUND)
      message(SEND_ERROR "CVS was not found. ${err_msg}")
      return()
    endif(NOT CVS_FOUND)

    set(args "-E" "chdir" "${dir}" "${CVS_EXECUTABLE}" "update" "-dP")

    add_custom_target(${target_name}
      ${CMAKE_COMMAND} ${args}
      )
  
  # Don't know the repository

  else(type STREQUAL "svn")

      message(SEND_ERROR "Unknown repository type. ${err_msg}")
      return()

  endif(type STREQUAL "svn")

endfunction(SLICER_CREATE_UPDATE_MODULE_TARGET)
