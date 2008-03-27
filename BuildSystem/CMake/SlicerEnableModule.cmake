cmake_minimum_required(VERSION 2.5)
include(SlicerSetGetModule)

# ---------------------------------------------------------------------------
# slicer_create_use_module_option: create an option to use a module.
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
#   slicer_parse_module_file("C:/foo/TestModule/TestModule.xml" TestModule)
#   slicer_create_use_module_option(TestModule USE_TEST_MODULE)
#   if(USE_TEST_MODULE)
#     ...
#   endif(USE_TEST_MODULE)
#
# See also:
#   slicer_get_module_value
#   slicer_set_module_value
#   slicer_get_module_short_description
# ---------------------------------------------------------------------------

function(slicer_create_use_module_option module_varname option_name)

  # Unknown module? Bail.

  slicer_is_module_unknown(
    ${module_varname} unknown "Unable to create option!")
  if(unknown)
    return()
  endif(unknown)

  # Create the option

  slicer_get_module_short_description(${module_varname} short_desc)
  option(${option_name} "Use ${short_desc}." OFF)

endfunction(slicer_create_use_module_option)

# ---------------------------------------------------------------------------
# slicer_create_use_modules_options: create options to use specific modules.
#
# This function can be used to create a boolean OPTION variable to control if
# a module should be used or not, for a list of specific modules.
#
# The option name will be created by prefixing the uppercase module name
# with "USE_". The option itself is created by calling 
# slicer_create_use_module_option
#
# Arguments:
# in:
#   modules (string): list of module variable names (*has* to be quoted)
# 
# Example:
#   slicer_parse_module_file("C:/foo/TestModule/TestModule.xml" TestModule)
#   slicer_parse_module_url("http://foo/bar/module/module2.xml" module2)
#   ...
#   slicer_get_modules_list(modules)
#   slicer_create_use_modules_options("${modules}")
#
# See also:
#   slicer_create_use_module_option
# ---------------------------------------------------------------------------

function(slicer_create_use_modules_options modules)

  foreach(module_varname ${modules})

    slicer_get_module_value(${module_varname} Name name)
    if(name)
      string(TOUPPER "${name}" upper_name)
      set(option_name "USE_${upper_name}")
      slicer_create_use_module_option(${module_varname} ${option_name})
    endif(name)
    
  endforeach(module_varname)

endfunction(slicer_create_use_modules_options)

# ---------------------------------------------------------------------------
# slicer_get_used_modules_list: get the list of used modules.
#
# This function can be used to retrieve the list of modules which 
# "USE_" option is set to ON, among a list of modules.
#
# Arguments:
# in:
#   modules (string): list of module variable names (*has* to be quoted)
# out:
#   list_varname (string): variable name to use to store the used modules list
# 
# Example:
#   slicer_parse_module_file("TestModule.xml" TestModule)
#   slicer_parse_module_file("TestModule2.xml" TestModule2)
#
#   slicer_get_modules_list(modules)
#   slicer_create_use_modules_options("${modules}")
#   slicer_get_used_modules_list("${modules}" used_modules)
#
# See also:
#   slicer_parse_module
#   slicer_parse_module_file
# ---------------------------------------------------------------------------

function(slicer_get_used_modules_list modules list_varname)

  set(used_modules)
  foreach(module_varname ${modules})

    slicer_get_module_value(${module_varname} Name name)
    if(name)
      string(TOUPPER "${name}" upper_name)
      set(option_name "USE_${upper_name}")
      if(${option_name})
        set(used_modules ${used_modules} ${module_varname})
      endif(${option_name})
    endif(name)
    
  endforeach(module_varname)

  set(${list_varname} ${used_modules} PARENT_SCOPE)
  
endfunction(slicer_get_used_modules_list)

# ---------------------------------------------------------------------------
# slicer_get_unresolved_modules_dependencies: get the unresolved modules dependencies for specific modules 
#
# This function can be used to retrieve the list of unique unresolved modules 
# dependencies among a list of specific modules; this is the list of 
# dependent modules that are not part of the list of modules passed as 
# parameter itself.
#
# Arguments:
# in:
#   modules (string): list of module variable names (*has* to be quoted)
# out:
#   list_varname (string): variable name to use to store the unresolved list
# 
# Example:
#   slicer_parse_module_file("C:/foo/TestModule/TestModule.xml" TestModule)
#   slicer_parse_module_url("http://foo/bar/module/module2.xml" module2)
#   ...
#   slicer_get_modules_list(modules)
#   slicer_create_use_modules_options("${modules}")
#   slicer_get_used_modules_list("${modules}" used_modules)
#   slicer_get_unresolved_modules_dependencies("${used_modules}" unresolved_modules)
#
# See also:
#   slicer_create_use_module_option
# ---------------------------------------------------------------------------

function(slicer_get_unresolved_modules_dependencies modules list_varname)

  set(unresolved_modules)

  foreach(module_varname ${modules})

    slicer_get_module_value(${module_varname} Dependency dependencies)
    if(dependencies)
      foreach(dependency ${dependencies})

        list(FIND "${modules}" ${dependency} index)
        if(index EQUAL -1)
          set(unresolved_modules ${unresolved_modules} ${dependency})
        endif(index EQUAL -1)

      endforeach(dependency)
    endif(dependencies)
    
  endforeach(module_varname)

  set(${list_varname} ${unresolved_modules} PARENT_SCOPE)

endfunction(slicer_get_unresolved_modules_dependencies)

