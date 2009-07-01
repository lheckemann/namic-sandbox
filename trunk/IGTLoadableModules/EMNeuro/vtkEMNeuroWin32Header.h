/*==========================================================================

Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $HeadURL: $
Date:      $Date: $
Version:   $Revision: $

==========================================================================*/

#ifndef __vtkEMNeuroWin32Header_h
#define __vtkEMNeuroWin32Header_h

#include <vtkEMNeuroConfigure.h>

#if defined(WIN32) && !defined(VTKSLICER_STATIC)
#if defined(EMNeuro_EXPORTS)
#define VTK_EMNeuro_EXPORT __declspec( dllexport ) 
#else
#define VTK_EMNeuro_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_EMNeuro_EXPORT 
#endif
#endif
