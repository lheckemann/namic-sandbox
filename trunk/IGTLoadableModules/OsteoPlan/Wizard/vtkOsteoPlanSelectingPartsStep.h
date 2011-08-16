/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#ifndef __vtkOsteoPlanSelectingPartsStep_h
#define __vtkOsteoPlanSelectingPartsStep_h

#include "vtkOsteoPlanStep.h"

class vtkSlicerInteractorStyle;
class vtkKWEntryWithLabel;
class vtkKWFrame;
class vtkKWFrameWithLabel;
class vtkKWLabel;
class vtkKWPushButton;
class vtkSlicerNodeSelectorWidget;
class vtkMRMLModelNode;
class vtkStringArray;

class VTK_OsteoPlan_EXPORT vtkOsteoPlanSelectingPartsStep : public vtkOsteoPlanStep
{
 public:
  static vtkOsteoPlanSelectingPartsStep *New();
  vtkTypeRevisionMacro(vtkOsteoPlanSelectingPartsStep,vtkOsteoPlanStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();
  virtual void HideUserInterface();
  virtual void TearDownGUI();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);
  virtual void ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData) {};
  virtual void UpdateGUI();

  void HandleMouseEvent(vtkSlicerInteractorStyle* style);
  void SelectClickedPart();

 protected:
  vtkOsteoPlanSelectingPartsStep();
  ~vtkOsteoPlanSelectingPartsStep();

  void AddGUIObservers();
  void RemoveGUIObservers();

  bool ProcessingCallback;

  vtkKWFrame                     *MainFrame;
  vtkKWFrameWithLabel            *SelectFrame;
  vtkKWLabel                     *InputModelLabel;
  vtkSlicerNodeSelectorWidget    *InputModelSelector;
  vtkMRMLModelNode               *InputModel;
  vtkKWEntryWithLabel            *PartNameEntry;
  vtkKWPushButton                *SelectPartButton;

  bool                            SelectingPart;

  vtkStringArray*                 ColorName;
  double                         *colorId[7];
  int                             ColorNumber;

 private:
  vtkOsteoPlanSelectingPartsStep(const vtkOsteoPlanSelectingPartsStep&);
  void operator=(const vtkOsteoPlanSelectingPartsStep&);

};

#endif

