/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkPerkStationModuleLogic.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
#ifndef __vtkPerkStationModuleLogic_h
#define __vtkPerkStationModuleLogic_h

#include "vtkSlicerModuleLogic.h"
#include "vtkMRMLScene.h"

#include "vtkPerkStationModule.h"
#include "vtkMRMLPerkStationModuleNode.h"


class VTK_PERKSTATIONMODULE_EXPORT vtkPerkStationModuleLogic : public vtkSlicerModuleLogic
{
  public:
  static vtkPerkStationModuleLogic *New();
  vtkTypeMacro(vtkPerkStationModuleLogic,vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

  // TODO: do we need to observe MRML here?
  virtual void ProcessMrmlEvents ( vtkObject *caller, unsigned long event,
                                   void *callData ){};

  // Description: Get/Set MRML node storing parameter values
  vtkGetObjectMacro (PerkStationModuleNode, vtkMRMLPerkStationModuleNode);
  void SetAndObservePerkStationModuleNode(vtkMRMLPerkStationModuleNode *n) 
    {
    vtkSetAndObserveMRMLNodeMacro( this->PerkStationModuleNode, n);
    }

  bool ReadConfigFile(istream &file);
  static char *strrev(char *s,int n);
protected:
  vtkPerkStationModuleLogic();
  virtual ~vtkPerkStationModuleLogic();
  vtkPerkStationModuleLogic(const vtkPerkStationModuleLogic&);
  void operator=(const vtkPerkStationModuleLogic&);

  vtkMRMLPerkStationModuleNode* PerkStationModuleNode;

};

#endif

