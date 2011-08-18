/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#ifndef __vtkOsteoPlanReturningOriginalPositionStep_h
#define __vtkOsteoPlanReturningOriginalPositionStep_h

#include "vtkOsteoPlanStep.h"

class vtkSlicerInteractorStyle;
class vtkKWEntryWithLabel;
class vtkKWFrame;
class vtkKWFrameWithLabel;
class vtkKWLabel;
class vtkKWPushButton;
class vtkSlicerNodeSelectorWidget;
class vtkMRMLModelNode;
class vtkMRMLLinearTransformNode;

class VTK_OsteoPlan_EXPORT vtkOsteoPlanReturningOriginalPositionStep : public vtkOsteoPlanStep
{
 public:
  static vtkOsteoPlanReturningOriginalPositionStep *New();
  vtkTypeRevisionMacro(vtkOsteoPlanReturningOriginalPositionStep,vtkOsteoPlanStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();
  virtual void HideUserInterface();
  virtual void TearDownGUI();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);
  virtual void ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData) {};
  virtual void UpdateGUI();

  void HandleMouseEvent(vtkSlicerInteractorStyle* style);
  void BackModelToOriginalPosition(vtkMRMLModelNode* Model);
  //  void BackModelToOriginalPosition(vtkMRMLLinearTransformNode *ParentTransform, vtkMRMLModelNode* Model);    // Old function

 protected:
  vtkOsteoPlanReturningOriginalPositionStep();
  ~vtkOsteoPlanReturningOriginalPositionStep();

  void AddGUIObservers();
  void RemoveGUIObservers();

  bool ProcessingCallback;

  vtkKWFrame                  *MainFrame;
  vtkSlicerNodeSelectorWidget *modelSelector;
  vtkKWPushButton             *BackOriginalButton;
  vtkMRMLModelNode            *SelectedModel;

 private:
  vtkOsteoPlanReturningOriginalPositionStep(const vtkOsteoPlanReturningOriginalPositionStep&);
  void operator=(const vtkOsteoPlanReturningOriginalPositionStep&);

};

#endif

