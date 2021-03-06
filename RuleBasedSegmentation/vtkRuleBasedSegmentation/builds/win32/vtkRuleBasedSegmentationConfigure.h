/* 
 * Here is where system computed values get stored.
 * These values should only change when the target compile platform changes.
 */

#define VTKRULEBASEDSEGMENTATION_BUILD_SHARED_LIBS
#ifndef VTKRULEBASEDSEGMENTATION_BUILD_SHARED_LIBS
#define VTKRULEBASEDSEGMENTATION_STATIC
#endif

#if defined(WIN32) && !defined(VTKRULEBASEDSEGMENTATION_STATIC)
#pragma warning ( disable : 4275 )

#if defined(vtkRuleBasedSegmentation_EXPORTS)
#define VTK_RULEBASEDSEGMENTATION_EXPORT __declspec( dllexport ) 
#else
#define VTK_RULEBASEDSEGMENTATION_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_RULEBASEDSEGMENTATION_EXPORT
#endif
