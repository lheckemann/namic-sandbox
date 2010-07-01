/*==========================================================================

Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $HeadURL: $
Date:      $Date: $
Version:   $Revision: $

==========================================================================*/

/* 
 * Here is where system computed values get stored.
 * These values should only change when the target compile platform changes.
 */

#if defined(WIN32) && !defined(VTKSLICER_STATIC)
  #pragma warning ( disable : 4275 )
#endif

#define BUILD_SHARED_LIBS
#ifndef BUILD_SHARED_LIBS
  #define VTKSLICER_STATIC
#endif
