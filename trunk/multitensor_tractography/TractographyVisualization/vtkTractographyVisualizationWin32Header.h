#ifndef __vtkTractographyVisualizationWin32Header_h
#define __vtkTractographyVisualizationWin32Header_h

#include <vtkTractographyVisualizationConfigure.h>

#if defined(WIN32) && !defined(VTKSLICER_STATIC)
#if defined(TractographyVisualization_EXPORTS)
#define VTK_TRACTOGRAPHYVISUALIZATION_EXPORT __declspec(dllexport) 
#else
#define VTK_TRACTOGRAPHYVISUALIZATION_EXPORT __declspec(dllimport) 
#endif
#else
#define VTK_TRACTOGRAPHYVISUALIZATION_EXPORT 
#endif

#endif
