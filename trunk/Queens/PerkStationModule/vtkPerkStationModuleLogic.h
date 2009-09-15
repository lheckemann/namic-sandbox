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

class vtkSlicerApplication;
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

   // Description:
   // Add volume to MRML scene and return the MRML node.
   // If volumeType is specified, then the volume is also selected as the current Calibration
   // targeting or verification volume
  vtkMRMLScalarVolumeNode *AddVolumeToScene(vtkSlicerApplication* app,const char *fileName, VolumeType volumeType=VOL_GENERIC);  
  static void ComputePermutationFromOrientation(vtkMatrix4x4 *matrix, int permutation[3], int flip[3]);
  bool ReadConfigFile(istream &file);
  static char *strrev(char *s,int n);
  static bool DoubleEqual(double val1, double val2);
protected:

  // Description:
  // Helper method for loading a volume via the Volume module.
  vtkMRMLScalarVolumeNode *AddArchetypeVolume(vtkSlicerApplication* app, const char* fileName, const char *volumeName);  
   // Description:
   // Set window/level computation, forcing the volume type to scalar (not statistical)
   void SetAutoScaleScalarVolume(vtkMRMLScalarVolumeNode *volumeNode);

  vtkPerkStationModuleLogic();
  virtual ~vtkPerkStationModuleLogic();
  vtkPerkStationModuleLogic(const vtkPerkStationModuleLogic&);
  void operator=(const vtkPerkStationModuleLogic&);

  vtkMRMLPerkStationModuleNode* PerkStationModuleNode;
 private:
 // Description:
 // Set Slicers's 2D view orientations from the image orientation.
 void SetSliceViewFromVolume(vtkSlicerApplication *app, vtkMRMLVolumeNode *volumeNode);
};

#endif

