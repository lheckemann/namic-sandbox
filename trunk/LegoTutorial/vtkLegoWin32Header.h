
#ifndef __vtkLegoWin32Header_h
#define __vtkLegoWin32Header_h

#include <vtkLegoConfigure.h>

#if defined(WIN32) && !defined(VTKSLICER_STATIC)
#if defined(Lego_EXPORTS)
#define VTK_LEGO_EXPORT __declspec( dllexport ) 
#else
#define VTK_LEGO_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_LEGO_EXPORT 
#endif
#endif
