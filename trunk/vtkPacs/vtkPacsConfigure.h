/* 
 * Here is where system computed values get stored.
 * These values should only change when the target compile platform changes.
 */

#define VTKPACS_BUILD_SHARED_LIBS
#ifndef VTKPACS_BUILD_SHARED_LIBS
#define VTKPACS_STATIC
#endif

#if defined(WIN32) && !defined(VTKPACS_STATIC)
#pragma warning ( disable : 4275 )

#if defined(vtkPacs_EXPORTS)
#define VTK_PACS_EXPORT __declspec( dllexport ) 
#else
#define VTK_PACS_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_PACS_EXPORT
#endif
