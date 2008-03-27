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
      slicer_create_use_module_option(${module_varname} "USE_${upper_name}")
    endif(name)
    
  endforeach(module_varname)

endfunction(slicer_create_use_modules_options)

# ---------------------------------------------------------------------------
# slicer_check_modules_dependencies: check dependencies for specific modules 
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
#   slicer_check_modules_dependencies("${modules}")
#
# See also:
#   slicer_create_use_module_option
# ---------------------------------------------------------------------------

function(slicer_check_modules_dependencies modules)

  foreach(module_varname ${modules})

  endforeach(module_varname)

endfunction(slicer_check_modules_dependencies)

