/*==========================================================================

Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $HeadURL: $
Date:      $Date: $
Version:   $Revision: $

==========================================================================*/

#ifndef __vtkMTIPlanWin32Header_h
#define __vtkMTIPlanWin32Header_h

#include <vtkMTIPlanConfigure.h>

#if defined(WIN32) && !defined(VTKSLICER_STATIC)
#if defined(MTIPlan_EXPORTS)
#define VTK_MTIPlan_EXPORT __declspec( dllexport ) 
#else
#define VTK_MTIPlan_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_MTIPlan_EXPORT 
#endif
#endif
