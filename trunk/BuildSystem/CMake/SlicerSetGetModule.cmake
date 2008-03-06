CMAKE_MINIMUM_REQUIRED(VERSION 2.5)

# ---------------------------------------------------------------------------
# SLICER_SET_MODULE_VALUE: Set a module value.
#
# This function can be used to set a module value without worrying too much
# about the underlying data structure. 
# Implementation: values are stored as key/value pairs at the moment.
#
# Arguments:
# in:
#   key (string): key
#   value (string): value
# in/out:
#   module_varname (string): variable name to use to store the module value
# 
# Example:
#   SLICER_SET_MODULE_VALUE(TestModule "Author" "John Doe")
#
# See also:
#   SLICER_GET_MODULE_VALUE
#   SLICER_UNSET_MODULE_VALUE
# ---------------------------------------------------------------------------

FUNCTION(SLICER_SET_MODULE_VALUE module_varname key value)
  
  SET_PROPERTY(GLOBAL PROPERTY "_${module_varname}_${key}" ${value})

ENDFUNCTION(SLICER_SET_MODULE_VALUE)

# ---------------------------------------------------------------------------
# SLICER_GET_MODULE_VALUE: Get a module value.
#
# This function can be used to retrieve a module value without worrying too much
# about the underlying data structure. 
# Implementation: values are stored as key/value pairs at the moment.
# If the module has no value for that key, the resulting variable is unset.
#
# Arguments:
# in:
#   module_varname (string): variable name used to store the module values
#   key (string): key
# in/out:
#   value_varname (string): variable name to use to store the specific value
# 
# Example:
#   SLICER_GET_MODULE_VALUE(TestModule "Author" authors)
#   MESSAGE("Author(s): ${author}")
#
# See also:
#   SLICER_SET_MODULE_VALUE
#   SLICER_UNSET_MODULE_VALUE
# ---------------------------------------------------------------------------

FUNCTION(SLICER_GET_MODULE_VALUE module_varname key value_varname)

  GET_PROPERTY(defined GLOBAL PROPERTY "_${module_varname}_${key}" DEFINED)

  IF(defined)
    GET_PROPERTY(value GLOBAL PROPERTY "_${module_varname}_${key}")
    SET(${value_varname} ${value} PARENT_SCOPE)
  ELSE(defined)
    SET(${value_varname} PARENT_SCOPE)
  ENDIF(defined)

ENDFUNCTION(SLICER_GET_MODULE_VALUE)

# ---------------------------------------------------------------------------
# SLICER_UNSET_MODULE_VALUE: unset a module value.
#
# This function can be used to unset a module value without worrying too much
# about the underlying data structure. 
# Implementation: values are stored as key/value pairs at the moment.
#
# Arguments:
# in:
#   key (string): key
# in/out:
#   module_varname (string): variable name used to store the module values
# 
# Example:
#   SLICER_UNSET_MODULE_VALUE(TestModule "Author")
#
# See also:
#   SLICER_GET_MODULE_VALUE
#   SLICER_SET_MODULE_VALUE
# ---------------------------------------------------------------------------

FUNCTION(SLICER_UNSET_MODULE_VALUE module_varname key)
  
  SET_PROPERTY(GLOBAL PROPERTY "_${module_varname}_${key}")

ENDFUNCTION(SLICER_UNSET_MODULE_VALUE)

# ---------------------------------------------------------------------------
# SLICER_IS_MODULE_UNKNOWN: check if a module is unknown.
#
# This function can be used to check if a module is unknown, i.e. if
# some of its core values are not in memory (most likely the module name was
# mistyped, or the module was not parsed yet).
# If the module is unknown and an optional error message is passed, it will
# be output using MESSAGE(SEND_ERROR ...) .
#
# Arguments:
# in:
#   module_varname (string): variable name used to store the module values
# out:
#   bool_varname (string): variable name to store if module is unknown
# optional in:
#   error_msg (string): optional error message
# 
# Example:
#   SLICER_IS_MODULE_UNKNOWN(TestModule unknown "Can't do that!")
#
# See also:
#   SLICER_GET_MODULE_VALUE
#   SLICER_SET_MODULE_VALUE
# ---------------------------------------------------------------------------

FUNCTION(SLICER_IS_MODULE_UNKNOWN module_varname bool_varname)
  
  SLICER_GET_MODULE_VALUE(${module_varname} "Name" name)
  IF(NOT name)
    SET(${bool_varname} 1 PARENT_SCOPE)
    IF(ARGN)
      MESSAGE(SEND_ERROR "Unknown module ${module_varname}. ${ARGN}")
    ENDIF(ARGN)
  ELSE(NOT name)
    SET(${bool_varname} 0 PARENT_SCOPE)
  ENDIF(NOT name)

ENDFUNCTION(SLICER_IS_MODULE_UNKNOWN)

# ---------------------------------------------------------------------------
# SLICER_GET_MODULE_SHORT_DESCRIPTION: Get a module short description.
#
# This function uses the module variables to create a short module description.
#
# Arguments:
# in:
#   module_varname (string): variable name used to store the module keys/values
# out:
#   desc_varname (string): variable name to use to store the short description
# 
# Example:
#   SLICER_GET_MODULE_SHORT_DESCRIPTION(TestModule desc)
#   MESSAGE("TestModule Short Description: ${desc}")
#
# See also:
#   SLICER_GET_MODULE_VALUE
#   SLICER_SET_MODULE_VALUE
# ---------------------------------------------------------------------------

FUNCTION(SLICER_GET_MODULE_SHORT_DESCRIPTION module_varname desc_varname)

  # Unknown module? Bail.

  SLICER_IS_MODULE_UNKNOWN(
    ${module_varname} unknown "Unable to create short description!")
  IF(unknown)
    RETURN()
  ENDIF(unknown)

  # Create the short description by assembling various values

  SLICER_GET_MODULE_VALUE(${module_varname} "Name" name)
  IF(name)
    SET(short_desc "${name}")
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
# SLICER_GET_MODULE_SOURCE_REPOSITORY_TYPE: Get a module source repository type.
#
# This function can be used to retrieve the type of source repository the 
# module is using.
# Will return either "cvs", "svn", or unset the var if unknown.
#
# Arguments:
# in:
#   module_varname (string): variable name used to store the module keys/values
# out:
#   type_varname (string): variable name to use to store the type
# 
# Example:
#   SLICER_GET_MODULE_SOURCE_REPOSITORY_TYPE(TestModule type)
#   IF(type STREQUAL "svn")
#     ...
#   ENDIF(type STREQUAL "svn")
#
# See also:
#   SLICER_GET_MODULE_VALUE
#   SLICER_SET_MODULE_VALUE
# ---------------------------------------------------------------------------

FUNCTION(SLICER_GET_MODULE_SOURCE_REPOSITORY_TYPE module_varname type_varname)

  # Unknown module? Bail.

  SLICER_IS_MODULE_UNKNOWN(
    ${module_varname} unknown "Unable to get repository type!")
  IF(unknown)
    RETURN()
  ENDIF(unknown)

  # Parse the SourceLocation for some hings about the repository type

  SLICER_GET_MODULE_VALUE(${module_varname} "SourceLocation" source_loc)
  IF(source_loc)
    STRING(REGEX MATCH "^http://.+$" found_svn "${source_loc}")
    IF(found_svn)
      SET(${type_varname} "svn" PARENT_SCOPE)
    ELSE(found_svn)
      STRING(REGEX MATCH "^:.+:.+:.+$" found_cvs "${source_loc}")
      IF(found_cvs)
        SET(${type_varname} "cvs" PARENT_SCOPE)
      ENDIF(found_cvs)
    ENDIF(found_svn)
  ELSE(source_loc)
    SET(${type_varname} PARENT_SCOPE)
  ENDIF(source_loc)

ENDFUNCTION(SLICER_GET_MODULE_SOURCE_REPOSITORY_TYPE)
