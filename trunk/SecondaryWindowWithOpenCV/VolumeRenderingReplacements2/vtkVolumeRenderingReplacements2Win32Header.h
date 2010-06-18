#ifndef __vtkVolumeRenderingReplacements2Win32Header_h
        #define __vtkVolumeRenderingReplacements2Win32Header_h

        #include <vtkVolumeRenderingReplacements2Configure.h>

        #if defined(WIN32) && !defined(VTKSLICER_STATIC)
                #if defined(VolumeRenderingReplacements2_EXPORTS)
                        #define VTK_VOLUMERENDERINGREPLACEMENTS_EXPORT __declspec( dllexport ) 
                #else
                        #define VTK_VOLUMERENDERINGREPLACEMENTS_EXPORT __declspec( dllimport ) 
                #endif
        #else
                #define VTK_VOLUMERENDERINGREPLACEMENTS_EXPORT 
        #endif
#endif
