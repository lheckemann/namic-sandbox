/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkPerkStationModuleLogic.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"

#include "vtkPerkStationModuleLogic.h"
#include "vtkPerkStationModule.h"

#include "vtkMRMLScene.h"
#include "vtkMRMLScalarVolumeNode.h"


vtkPerkStationModuleLogic* vtkPerkStationModuleLogic::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkPerkStationModuleLogic");
  if(ret)
    {
      return (vtkPerkStationModuleLogic*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkPerkStationModuleLogic;
}


//----------------------------------------------------------------------------
vtkPerkStationModuleLogic::vtkPerkStationModuleLogic()
{
  this->PerkStationModuleNode = NULL;
}

//----------------------------------------------------------------------------
vtkPerkStationModuleLogic::~vtkPerkStationModuleLogic()
{
  vtkSetMRMLNodeMacro(this->PerkStationModuleNode, NULL);
}

//----------------------------------------------------------------------------
void vtkPerkStationModuleLogic::PrintSelf(ostream& os, vtkIndent indent)
{
}
