/*=========================================================================

  Program:   AtamaiTracking for VTK
  Module:    $RCSfile: vtkTrackingWin32Header.h,v $
  Creator:   David Gobbi <dgobbi@atamai.com>
  Language:  C++
  Author:    $Author: dgobbi $
  Date:      $Date: 2005/07/01 22:52:05 $
  Version:   $Revision: 1.2 $

=========================================================================*/
// .NAME vtkTrackingWin32Header - manage Windows system differences
// .SECTION Description
// The vtkTrackingWin32Header captures some system differences between Unix
// and Windows operating systems.

#ifndef __vtkTrackingWin32Header_h
#define __vtkTrackingWin32Header_h

#include <QueensOpenIGTLibsConfigure.h>

#if defined(WIN32) && !defined(QUEENSOPENIGT_STATIC)
#if defined(vtkOpenIGTTracking_EXPORTS)
#define VTK_TRACKING_EXPORT __declspec( dllexport )
#else
#define VTK_TRACKING_EXPORT __declspec( dllimport )
#endif
#else
#define VTK_TRACKING_EXPORT
#endif

#endif

