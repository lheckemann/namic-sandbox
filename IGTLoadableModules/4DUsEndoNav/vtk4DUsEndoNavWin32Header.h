/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtk4DUsEndoNavWin32Header_h
#define __vtk4DUsEndoNavWin32Header_h

#include "vtk4DUsEndoNavConfigure.h"

#if defined(WIN32) && !defined(VTKSLICER_STATIC)
  #if defined(4DUsEndoNav_EXPORTS)
    #define VTK_4DUsEndoNav_EXPORT __declspec( dllexport )
  #else
    #define VTK_4DUsEndoNav_EXPORT __declspec( dllimport )
  #endif
#else
  #define VTK_4DUsEndoNav_EXPORT
#endif

#endif
