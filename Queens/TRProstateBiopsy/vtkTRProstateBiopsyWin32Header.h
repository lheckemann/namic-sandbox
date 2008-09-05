#ifndef __vtkTRProstateBiopsyWin32Header_h
#define __vtkTRProstateBiopsyWin32Header_h

#include <vtkTRProstateBiopsyConfigure.h>

#if defined(WIN32) && !defined(VTKSLICER_STATIC)
#if defined(TRProstateBiopsy_EXPORTS)
#define VTK_TRPROSTATEBIOPSY_EXPORT __declspec( dllexport ) 
#else
#define VTK_TRPROSTATEBIOPSY_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_TRPROSTATEBIOPSY_EXPORT 
#endif
#endif
