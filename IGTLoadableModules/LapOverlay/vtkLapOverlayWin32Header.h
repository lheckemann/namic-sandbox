/*==========================================================================

Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $HeadURL: $
Date:      $Date: $
Version:   $Revision: $

==========================================================================*/

#ifndef __vtkLapOverlayWin32Header_h
#define __vtkLapOverlayWin32Header_h

#include <vtkLapOverlayConfigure.h>

#if defined(WIN32) && !defined(VTKSLICER_STATIC)
#if defined(LapOverlay_EXPORTS)
#define VTK_LapOverlay_EXPORT __declspec( dllexport ) 
#else
#define VTK_LapOverlay_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_LapOverlay_EXPORT 
#endif
#endif
