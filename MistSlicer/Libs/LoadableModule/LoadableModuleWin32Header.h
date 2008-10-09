/*=========================================================================

  Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   Module Description Parser
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Libs/LoadableModule/LoadableModuleWin32Header.h $
  Date:      $Date: 2008-03-05 13:16:09 +0900 (水, 05 3 2008) $
  Version:   $Revision: 6069 $

==========================================================================*/
// .NAME LoadableModuleWin32Header - manage Windows system differences
// .SECTION Description
// The LoadableModuleWin32Header captures some system differences between Unix
// and Windows operating systems. 

#ifndef __LoadableModuleWin32Header_h
#define __LoadableModuleWin32Header_h

#include <LoadableModuleConfigure.h>

#if defined(WIN32) && !defined(LoadableModule_STATIC)
#if defined(LoadableModule_EXPORTS)
#define LoadableModule_EXPORT __declspec( dllexport ) 
#else
#define LoadableModule_EXPORT __declspec( dllimport ) 
#endif
#else
#define LoadableModule_EXPORT 
#endif

#if defined(_MSC_VER)
#  pragma warning (disable: 4251) /* missing DLL-interface */
#endif

#endif
