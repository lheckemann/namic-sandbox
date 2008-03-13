cmake_minimum_required(VERSION 2.5)
include(SlicerSetGetModule)
find_package(Subversion)
find_package(CVS)

# ---------------------------------------------------------------------------
# slicer_create_download_module_target: Create a download module target.
#
# This function can be used to create a target to download a module
# repository (CVS or SVN).
#
# Note: while one could create multiple download targets with different 
# names, it is assumed that only one is needed; the name of this download
# target can be later on retrieved using slicer_get_download_module_target.
#
# Arguments:
# in:
#   module_varname (string): variable name used to store the module values
#   target_name (string): name of the target
#   dir (string): where the module should be downloaded
# 
# Example:
#   slicer_create_download_module_target(
#       TestModule testmodule_download "/src/")
#
# See also:
#   slicer_get_download_module_target
#   slicer_create_update_module_target
# ---------------------------------------------------------------------------

function(slicer_create_download_module_target module_varname target_name dir)

  # Unknown module? Bail.

  set(err_msg "Unable to create download target!")
  slicer_is_module_unknown(${module_varname} unknown ${err_msg})
  if(unknown)
    return()
  endif(unknown)

  # Missing source location? Bye.

  slicer_get_module_value(${module_varname} SourceLocation source_loc)
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
    
    set(svn_args "checkout" "${source_loc}" "${dir}")

    add_custom_command(
      OUTPUT "${dir}/.svn/entries"
      COMMAND "${Subversion_SVN_EXECUTABLE}"
      ARGS ${svn_args}
      )
    add_custom_target(${target_name} DEPENDS "${dir}/.svn/entries")

    slicer_set_module_value(
      ${module_varname} __DownloadTarget__ ${target_name})

  # If CVS create a command to checkout the repository, assume it is
  # there if CVS/ sub-directory can be found.
  # Use the CVSModule parameter to get the module to checkout
  # Use the optional CVSBranch parameter to get the branch to checkout

  elseif(type STREQUAL "cvs")

    if(NOT CVS_FOUND)
      message(SEND_ERROR "CVS was not found. ${err_msg}")
      return()
    endif(NOT CVS_FOUND)

    slicer_get_module_value(${module_varname} CVSModule cvs_module)
    if(NOT cvs_module)
      message(SEND_ERROR 
        "Unknown CVS module for module ${module_varname}. ${err_msg}")
      return()
    endif(NOT cvs_module)

    # Make sure the directory exists (actually only the parent needs to exist)

    set(cmake_args "-E" "make_directory" "${dir}")

    add_custom_command(
      OUTPUT "${dir}"
      COMMAND "${CMAKE_COMMAND}"
      ARGS ${cmake_args}
      )

    # Checkout

    set(cvs_args "-d" "${source_loc}" "checkout" "-d" "${dir}")

    slicer_get_module_value(${module_varname} CVSBranch cvs_branch)
    if(cvs_branch)
      set(cvs_args ${cvs_args} "-r" "${cvs_branch}")
    endif(cvs_branch)

    set(cvs_args ${cvs_args} "${cvs_module}")

    add_custom_command(
      OUTPUT "${dir}/CVS/Root"
      DEPENDS "${dir}"
      COMMAND "${CVS_EXECUTABLE}"
      ARGS ${cvs_args}
      )

    add_custom_target(${target_name} DEPENDS "${dir}/CVS/Root")
  
    slicer_set_module_value(
      ${module_varname} __DownloadTarget__ ${target_name})

  # Don't know the repository

  else(type STREQUAL "svn")

      message(SEND_ERROR "Unknown repository type. ${err_msg}")
      return()

    slicer_unset_module_value(
      ${module_varname} __DownloadTarget__)

  endif(type STREQUAL "svn")

endfunction(slicer_create_download_module_target)

# ---------------------------------------------------------------------------
# slicer_get_download_module_target: get the name of a download module target.
#
# This function can be used to retrieve the name of the download target
# associated to a module, if any.
#
# Arguments:
# in:
#   module_varname (string): variable name used to store the module values
# out:
#   target_varname (string): variable name to use to store the target name
# 
# Example:
#   slicer_get_download_module_target(TestModule download_target_name)
#   message("download target for TestModule: ${download_target_name}")
#
# See also:
#   slicer_create_download_module_target
# ---------------------------------------------------------------------------

function(slicer_get_download_module_target module_varname target_varname)

  # Unknown module? Bail.

  set(err_msg "Unable to get download module target!")
  slicer_is_module_unknown(${module_varname} unknown ${err_msg})
  if(unknown)
    return()
  endif(unknown)

  slicer_get_module_value(${module_varname} __DownloadTarget__ value)
  set(${target_varname} ${value} PARENT_SCOPE)

endfunction(slicer_get_download_module_target)

# ---------------------------------------------------------------------------
# slicer_create_update_module_target: Create an update module target.
#
# This function can be used to create a target to update a module 
# repository (CVS or SVN).
#
# Note: while one could create multiple update targets with different 
# names, it is assumed that only one is needed; the name of this update
# target can be later on retrieved using slicer_get_update_module_target.
#
# Note: this function does not automatically create a download target. This
# can however be done using the add_dependencies() command; the name of
# the download target itself, if any for that module, can be retrieved using
# slicer_get_download_module_target.
#
# Arguments:
# in:
#   module_varname (string): variable name used to store the module values
#   target_name (string): name of the target
#   dir (string): where the module has been downloaded/checked out
# 
# Example:
#   slicer_create_update_module_target(
#       TestModule testmodule_update "/src/")
#
# See also:
#   slicer_create_download_module_target
#   slicer_get_update_module_target
# ---------------------------------------------------------------------------

function(slicer_create_update_module_target module_varname target_name dir)

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
    
    set(svn_args "update" "--non-interactive" "${dir}")

    add_custom_target(${target_name}
      "${Subversion_SVN_EXECUTABLE}" ${svn_args}
      )

    slicer_set_module_value(
      ${module_varname} __UpdateTarget__ ${target_name})

  # If CVS create a command to update the repository.
  # CVS can not update a specific directory, one has to chdir in the directory
  # to update. Use the CMake command-line client in Execute mode (-E) to
  # change to the directory, and run the CVS update command.

  elseif(type STREQUAL "cvs")

    if(NOT CVS_FOUND)
      message(SEND_ERROR "CVS was not found. ${err_msg}")
      return()
    endif(NOT CVS_FOUND)

    set(cmake_args "-E" "chdir" "${dir}" "${CVS_EXECUTABLE}" "update" "-dP")

    add_custom_target(${target_name}
      ${CMAKE_COMMAND} ${cmake_args}
      )
  
    slicer_set_module_value(
      ${module_varname} __UpdateTarget__ ${target_name})

  # Don't know the repository

  else(type STREQUAL "svn")

      message(SEND_ERROR "Unknown repository type. ${err_msg}")
      return()

    slicer_unset_module_value(
      ${module_varname} __UpdateTarget__)

  endif(type STREQUAL "svn")

endfunction(slicer_create_update_module_target)

# ---------------------------------------------------------------------------
# slicer_get_update_module_target: get the name of a update module target.
#
# This function can be used to retrieve the name of the update target
# associated to a module, if any.
#
# Arguments:
# in:
#   module_varname (string): variable name used to store the module values
# out:
#   target_varname (string): variable name to use to store the target name
# 
# Example:
#   slicer_get_update_module_target(TestModule update_target_name)
#   message("update target for TestModule: ${update_target_name}")
#
# See also:
#   slicer_create_update_module_target
# ---------------------------------------------------------------------------

function(slicer_get_update_module_target module_varname target_varname)

  # Unknown module? Bail.

  set(err_msg "Unable to get update module target!")
  slicer_is_module_unknown(${module_varname} unknown ${err_msg})
  if(unknown)
    return()
  endif(unknown)

  slicer_get_module_value(${module_varname} __UpdateTarget__ value)
  set(${target_varname} ${value} PARENT_SCOPE)

endfunction(slicer_get_update_module_target)

# ---------------------------------------------------------------------------
# slicer_create_download_and_update_modules_targets: 
#                  create download and update targets for all known modules.
#
# This function can be used to create and connect both download and update
# targets for all known modules.
#
# The download target name will be created by appending "_download" to the
# module name, unless that target exists already.
#
# The update target name will be created by appending "_update" to the
# module name, unless that target exists already.
#
# Each module's update target will depend on the module's download target
# so that downloading occurs automatically when the repository is not found.
#
# A unique "download" and "update" target will be created that will
# depend on all modules download and update targets (respectively).
#
# Each module is downloaded/checked out in its own sub-directory, named after
# the module's name.
#
# Arguments:
# in:
#   dir (string): where the modules should be downloaded (parent dir)
# 
# Example:
#   slicer_get_update_module_target(TestModule update_target_name)
#
# See also:
#   slicer_create_update_module_target
# ---------------------------------------------------------------------------

function(slicer_create_download_and_update_modules_targets dir)

  # Create the global download and update targets

  add_custom_target(download)
  add_custom_target(update)

  # Retrieve all the modules parsed so far and iterate

  slicer_get_parsed_modules_list(modules)

  foreach(module ${modules})

    # Find and/or create download target
    
    slicer_get_download_module_target(${module} download_target)
    if(NOT download_target)
      set(download_target "${module}_download")
      slicer_create_download_module_target(
        ${module} ${download_target} "${dir}/${module}")
    endif(NOT download_target)
    add_dependencies(download ${download_target})

    # Find and/or create update target
    
    slicer_get_update_module_target(${module} update_target)
    if(NOT update_target)
      set(update_target "${module}_update")
      slicer_create_update_module_target(
        ${module} ${update_target} "${dir}/${module}")
    endif(NOT update_target)
    add_dependencies(update ${update_target})

    # Attach download to update

    add_dependencies(${update_target} ${download_target})
    
  endforeach(module)

endfunction(slicer_create_download_and_update_modules_targets)

