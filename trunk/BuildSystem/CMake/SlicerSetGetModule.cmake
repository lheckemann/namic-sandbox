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

# ---------------------------------------------------------------------------
# SLICER_GET_MODULE_SHORT_DESCRIPTION: Get a module short description.
#
# This macro uses the module variables to create a short module description.
#
# Arguments:
#   module_varname (string): variable name used to store the module keys/values
#   desc_varname (string):   variable name used to store the short description
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
# SLICER_GET_MODULE_SOURCE_REPOSITORY_TYPE: Get a module source repository type.
#
# This macro can be used to retrieve the type of source repository the 
# module is using.
# Will return either "cvs", "svn", or unset the var if unknown.
#
# Arguments:
#   module_varname (string): variable name used to store the module keys/values
#   type_varname (string):   variable name used to store the type
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
