/*=========================================================================

  Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   Module Description Parser
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Libs/ModuleDescriptionParser/ModuleProcessInformation.cxx $
  Date:      $Date: 2007-10-06 11:14:52 +0900 (土, 06 10 2007) $
  Version:   $Revision: 4551 $

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
