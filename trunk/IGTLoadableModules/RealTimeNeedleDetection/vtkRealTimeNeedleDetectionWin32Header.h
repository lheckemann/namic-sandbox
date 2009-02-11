/*==========================================================================

Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $HeadURL: $
Date:      $Date: $
Version:   $Revision: $

==========================================================================*/

#ifndef __vtkRealTimeNeedleDetectionWin32Header_h
#define __vtkRealTimeNeedleDetectionWin32Header_h

#include <vtkRealTimeNeedleDetectionConfigure.h>

#if defined(WIN32) && !defined(VTKSLICER_STATIC)
#if defined(RealTimeNeedleDetection_EXPORTS)
#define VTK_RealTimeNeedleDetection_EXPORT __declspec( dllexport ) 
#else
#define VTK_RealTimeNeedleDetection_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_RealTimeNeedleDetection_EXPORT 
#endif
#endif
