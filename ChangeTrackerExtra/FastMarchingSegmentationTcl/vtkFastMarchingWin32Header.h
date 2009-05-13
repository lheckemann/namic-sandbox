
#ifndef __vtkFastMarchingWin32Header_h
#define __vtkFastMarchingWin32Header_h

#include <vtkFastMarchingConfigure.h>

#if defined(WIN32) && !defined(VTKSLICER_STATIC)
#if defined(FastMarching_EXPORTS)
#define VTK_FASTMARCHING_EXPORT __declspec( dllexport ) 
#else
#define VTK_FASTMARCHING_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_FASTMARCHING_EXPORT 
#endif
#endif
