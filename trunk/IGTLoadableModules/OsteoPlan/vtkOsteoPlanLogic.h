/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

// .NAME vtkOsteoPlanLogic - slicer logic class for Locator module
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT.


#ifndef __vtkOsteoPlanLogic_h
#define __vtkOsteoPlanLogic_h

#include "vtkOsteoPlanWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"

class vtkIGTLConnector;
class vtkMRMLModelNode;

class VTK_OsteoPlan_EXPORT vtkOsteoPlanLogic : public vtkSlicerModuleLogic
{
 public:

  //BTX
  enum {
    StatusUpdateEvent       = 50001,
  };
  //ETX

 public:

  static vtkOsteoPlanLogic *New();
  vtkTypeRevisionMacro(vtkOsteoPlanLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);

  void ReduceOperation(vtkMRMLModelNode* Remesh);
  void RefineOperation(vtkMRMLModelNode* Remesh);

 protected:

  vtkOsteoPlanLogic();
  ~vtkOsteoPlanLogic();

  void operator=(const vtkOsteoPlanLogic&);
  vtkOsteoPlanLogic(const vtkOsteoPlanLogic&);
  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();
  vtkCallbackCommand *DataCallbackCommand;

 private:


};

#endif



