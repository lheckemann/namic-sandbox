/*==========================================================================

  Portions (c) Copyright 2010-2011 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/
/// vtkAbdoNavWin32Header - manages Windows system differences
///
/// The vtkAbdoNavWin32Header captures some system differences between Unix
/// and Windows operating systems.

#ifndef __vtkAbdoNavWin32Header_h
#define __vtkAbdoNavWin32Header_h

#include <vtkAbdoNavConfigure.h>

#if defined(WIN32) && !defined(VTKSLICER_STATIC)
#if defined(AbdoNav_EXPORTS)
  #define VTK_AbdoNav_EXPORT __declspec( dllexport )
#else
  #define VTK_AbdoNav_EXPORT __declspec( dllimport )
#endif // AbdoNav_EXPORTS
#else
  #define VTK_AbdoNav_EXPORT
#endif // WIN32, VTKSLICER_STATIC

#endif // __vtkAbdoNavWin32Header_h
