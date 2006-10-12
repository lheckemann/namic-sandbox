SET(CMAKE_C_COMPILER "gcc")
SET(CMAKE_C_COMPILER_ARG1 "")
SET(CMAKE_AR "/usr/bin/ar")
SET(CMAKE_RANLIB "/usr/bin/ranlib")
SET(CMAKE_COMPILER_IS_GNUCC 1)
SET(CMAKE_C_COMPILER_LOADED 1)
SET(CMAKE_COMPILER_IS_MINGW )
SET(CMAKE_COMPILER_IS_CYGWIN )
IF(CMAKE_COMPILER_IS_CYGWIN)
  SET(CYGWIN 1)
  SET(UNIX 1)
ENDIF(CMAKE_COMPILER_IS_CYGWIN)

SET(CMAKE_C_COMPILER_ENV_VAR "CC")

IF(CMAKE_COMPILER_IS_MINGW)
  SET(MINGW 1)
ENDIF(CMAKE_COMPILER_IS_MINGW)
SET(CMAKE_COMPILER_IS_GNUCC_RUN 1)
SET(CMAKE_C_SOURCE_FILE_EXTENSIONS c)
SET(CMAKE_C_IGNORE_EXTENSIONS h;H;o;O;obj;OBJ;def;DEF;rc;RC)
SET(CMAKE_C_LINKER_PREFERENCE None)
IF(UNIX)
  SET(CMAKE_C_OUTPUT_EXTENSION .o)
ELSE(UNIX)
  SET(CMAKE_C_OUTPUT_EXTENSION .obj)
ENDIF(UNIX)
# save the size of void* in case where cache is removed
# and the this file is still around
SET(CMAKE_SIZEOF_VOID_P 4)
