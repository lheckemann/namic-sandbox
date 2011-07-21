// /  BRAINSCommonLib2DWin32Header - manage Windows system differences
// /
// / The BRAINSCommonLib2DWin32Header captures some system differences between
// Unix
// / and Windows operating systems.

#ifndef __BRAINSCommonLib2DWin32Header_h
#define __BRAINSCommonLib2DWin32Header_h

#include <BRAINSCommonLib2D.h>

#if defined( WIN32 ) && !defined( BRAINSCommonLib2D_STATIC )
#  if defined( BRAINSCommonLib2D_EXPORTS )
#    define BRAINSCommonLib2D_EXPORT __declspec(dllexport)
#  else
#    define BRAINSCommonLib2D_EXPORT __declspec(dllimport)
#  endif
#else
#  define BRAINSCommonLib2D_EXPORT
#endif

#endif
