/*=========================================================================

  Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   Module Description Parser
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Libs/LoadableModule/ModuleProcessInformation.cxx $
  Date:      $Date: 2008-03-05 13:16:09 +0900 (水, 05 3 2008) $
  Version:   $Revision: 6069 $

==========================================================================*/
#include "ModuleProcessInformation.h"


std::ostream& operator<<(std::ostream &os, const ModuleProcessInformation &p)
{
  os << "  Abort: " << (short) p.Abort << std::endl;
  os << "  Progress: " << p.Progress << std::endl;
  os << "  Stage Progress: " << p.StageProgress << std::endl;
  os << "  ProgressMessage: " << p.ProgressMessage << std::endl;
  os << "  ElapsedTime: " << p.ElapsedTime << std::endl;

  return os;
}
