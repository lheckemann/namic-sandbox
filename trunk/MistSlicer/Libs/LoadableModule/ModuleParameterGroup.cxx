/*=========================================================================

  Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   Module Description Parser
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Libs/LoadableModule/ModuleParameterGroup.cxx $
  Date:      $Date: 2008-03-05 13:16:09 +0900 (水, 05 3 2008) $
  Version:   $Revision: 6069 $

==========================================================================*/

#include "ModuleParameterGroup.h"

ModuleParameterGroup
::ModuleParameterGroup(const ModuleParameterGroup &parameters)
{
  this->Label = parameters.Label;
  this->Description = parameters.Description;
  this->Parameters = parameters.Parameters;
  this->Advanced = parameters.Advanced;
}

void
ModuleParameterGroup
::operator=(const ModuleParameterGroup &parameters)
{
  this->Label = parameters.Label;
  this->Description = parameters.Description;
  this->Parameters = parameters.Parameters;
  this->Advanced = parameters.Advanced;
}

std::ostream & operator<<(std::ostream &os, const ModuleParameterGroup &group)
{ 
  os << "  Advanced: " << group.GetAdvanced() << std::endl;
  os << "  Label: " << group.GetLabel() << std::endl;
  os << "  Description: " << group.GetDescription() << std::endl;
  os << "  Parameters: " << std::endl;
  std::vector<ModuleParameter>::const_iterator it = group.GetParameters().begin();
  while (it != group.GetParameters().end())
    {
    os << *it;
    ++it;
    }
  return os;
}
