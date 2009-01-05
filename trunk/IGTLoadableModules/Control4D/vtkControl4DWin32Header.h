/*==========================================================================

Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $HeadURL$
Date:      $Date$
Version:   $Revision$

==========================================================================*/

#ifndef __vtkControl4DWin32Header_h
#define __vtkControl4DWin32Header_h

#include <vtkControl4DConfigure.h>

#if defined(WIN32) && !defined(VTKSLICER_STATIC)
#if defined(Control4D_EXPORTS)
#define VTK_Control4D_EXPORT __declspec( dllexport ) 
#else
#define VTK_Control4D_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_Control4D_EXPORT 
#endif
#endif
