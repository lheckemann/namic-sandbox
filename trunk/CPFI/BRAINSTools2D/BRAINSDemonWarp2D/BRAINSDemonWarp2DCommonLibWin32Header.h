// /  BRAINSDemonWarp2DCommonLibWin32Header - manage Windows system differences
// /
// / The BRAINSDemonWarp2DCommonLibWin32Header captures some system differences
// between Unix
// / and Windows operating systems.

#ifndef __BRAINSDemonWarp2DCommonLibWin32Header_h
#define __BRAINSDemonWarp2DCommonLibWin32Header_h

#include <BRAINSDemonWarp2DCommonLib.h>

#if defined( WIN32 ) && !defined( BRAINSDemonWarp2DCommonLib_STATIC )
#  if defined( BRAINSDemonWarp2DCommonLib_EXPORTS )
#    define BRAINSDemonWarp2DCommonLib_EXPORT __declspec(dllexport)
#  else
#    define BRAINSDemonWarp2DCommonLib_EXPORT __declspec(dllimport)
#  endif
#else
#  define BRAINSDemonWarp2DCommonLib_EXPORT
#endif

#endif
