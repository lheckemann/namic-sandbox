/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkTRProstateBiopsyWin32Header.h,v $
  Date:      $Date: 2008/11/06 17:56:51 $
  Version:   $Revision: 1.4 $

=========================================================================auto=*/
// .NAME vtkTRProstateBiopsyWin32Header - manage Windows system differences
// .SECTION Description
// The vtkTRProstateBiopsyWin32Header captures some system differences between Unix
// and Windows operating systems. 

#ifndef __vtkTRProstateBiopsyWin32Header_h
#define __vtkTRProstateBiopsyWin32Header_h

#include <vtkTRProstateBiopsyConfigure.h>

#if defined(WIN32) && !defined(VTKSLICER_STATIC)
#if defined(TRProstateBiopsy_EXPORTS)
#define VTK_TRPROSTATEBIOPSY_EXPORT __declspec( dllexport ) 
#else
#define VTK_TRPROSTATEBIOPSY_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_TRPROSTATEBIOPSY_EXPORT 
#endif
#endif
