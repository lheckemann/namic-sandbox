/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#ifndef __vtkOsteoPlanPlacingScrewsStep_h
#define __vtkOsteoPlanPlacingScrewsStep_h

#include "vtkOsteoPlanStep.h"

class vtkSlicerInteractorStyle;
class vtkKWEntryWithLabel;
class vtkKWFrame;
class vtkKWFrameWithLabel;
class vtkKWLabel;
class vtkKWPushButton;
class vtkKWScaleWithLabel;
class vtkKWListBox;
class vtkSlicerNodeSelectorWidget;
class vtkMRMLModelNode;
class vtkCollection;

class vtkCylinderWidget;

class VTK_OsteoPlan_EXPORT vtkOsteoPlanPlacingScrewsStep : public vtkOsteoPlanStep
{
 public:
  static vtkOsteoPlanPlacingScrewsStep *New();
  vtkTypeRevisionMacro(vtkOsteoPlanPlacingScrewsStep,vtkOsteoPlanStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();
  virtual void HideUserInterface();
  virtual void TearDownGUI();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);
  virtual void ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData);
  virtual void UpdateGUI();

  void HandleMouseEvent(vtkSlicerInteractorStyle* style);
  void MarkScrewPosition();

 protected:
  vtkOsteoPlanPlacingScrewsStep();
  ~vtkOsteoPlanPlacingScrewsStep();

  void AddGUIObservers();
  void RemoveGUIObservers();

  bool ProcessingCallback;

  vtkKWFrame                  *MainFrame;
  vtkSlicerNodeSelectorWidget *FiducialOnModel;
  vtkKWFrameWithLabel         *ListOfModelsFrame;
  vtkKWFrame                  *ButtonsFrame;
  vtkKWPushButton             *StartPlacingScrews;
  vtkMRMLModelNode            *SelectedModel;
  vtkKWScaleWithLabel         *ScrewDiameterScale;
  vtkKWScaleWithLabel         *ScrewHeightScale;
  vtkCylinderWidget           *ScrewCylinder;
  vtkKWPushButton             *ApplyScrewButton;
  double                       ScrewDiameter;
  double                       ScrewHeight;
  bool                         bPlacingScrews;

  vtkKWPushButton *AddModelButton;
  vtkKWPushButton *RemoveModelButton;
  vtkKWPushButton *ClearListButton;
  vtkKWListBox *ModelsToClip;

  vtkCollection *ModelsToClipCollection;

 private:
  vtkOsteoPlanPlacingScrewsStep(const vtkOsteoPlanPlacingScrewsStep&);
  void operator=(const vtkOsteoPlanPlacingScrewsStep&);

};

#endif

