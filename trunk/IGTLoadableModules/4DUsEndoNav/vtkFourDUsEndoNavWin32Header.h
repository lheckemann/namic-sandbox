/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkFourDUsEndoNavWin32Header_h
#define __vtkFourDUsEndoNavWin32Header_h

#include "vtkFourDUsEndoNavConfigure.h"

#if defined(WIN32) && !defined(VTKSLICER_STATIC)
  #if defined(FourDUsEndoNav_EXPORTS)
    #define VTK_FourDUsEndoNav_EXPORT __declspec( dllexport )
  #else
    #define VTK_FourDUsEndoNav_EXPORT __declspec( dllimport )
  #endif
#else
  #define VTK_FourDUsEndoNav_EXPORT
#endif

#endif
