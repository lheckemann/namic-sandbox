/*=========================================================================

  Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   Loadable Module
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Libs/LoadableModule/LoadableModuleDescriptionParser.h $
  Date:      $Date: 2008-06-26 00:58:36 +0900 (木, 26 6 2008) $
  Version:   $Revision: 7176 $

==========================================================================*/
#ifndef __LoadableModuleDescriptionParser_h
#define __LoadableModuleDescriptionParser_h

#include <string>

#include "LoadableModuleWin32Header.h"

class LoadableModuleDescription;

class LoadableModule_EXPORT LoadableModuleDescriptionParser
{
public:
  LoadableModuleDescriptionParser() {};
  ~LoadableModuleDescriptionParser() {};

  int Parse( const std::string& xml, LoadableModuleDescription& description);

  int ParseText( const std::string& txt, LoadableModuleDescription& description);
};

#endif
