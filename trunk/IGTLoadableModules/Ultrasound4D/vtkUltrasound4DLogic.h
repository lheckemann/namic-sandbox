/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

// .NAME vtkUltrasound4DLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkUltrasound4DLogic_h
#define __vtkUltrasound4DLogic_h

#include "vtkUltrasound4DWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"

class vtkIGTLConnector;
class vtkMRMLScalarVolumeNode;
class vtkCollection;
class vtkKWScale;

class VTK_Ultrasound4D_EXPORT vtkUltrasound4DLogic : public vtkSlicerModuleLogic 
{
 public:
  //BTX
  enum {  // Events
    StatusUpdateEvent       = 50001,
  };
  //ETX

 public:
  
  static vtkUltrasound4DLogic *New();
  
  vtkTypeRevisionMacro(vtkUltrasound4DLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);
  void CopyVolume(vtkMRMLScalarVolumeNode *destinationNode, vtkMRMLScalarVolumeNode *sourceNode);
  void PlayVolumes(vtkMRMLScalarVolumeNode *destinationNode, vtkCollection *ListOfVolumes, vtkKWScale *scale);

 protected:
  
  vtkUltrasound4DLogic();
  ~vtkUltrasound4DLogic();

  void operator=(const vtkUltrasound4DLogic&);
  vtkUltrasound4DLogic(const vtkUltrasound4DLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkCallbackCommand *DataCallbackCommand;

 private:


};

#endif


  
