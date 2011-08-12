/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkOsteoPlanCuttingModelStep_h
#define __vtkOsteoPlanCuttingModelStep_h

#include "vtkOsteoPlanStep.h"

// KWWidgets
class vtkKWFrame;
class vtkKWFrameWithLabel;
class vtkKWPushButton;

// VTK
class vtkBoxWidget2;
class vtkBoxRepresentation;

// Slicer
class vtkSlicerNodeSelectorWidget;
class vtkMRMLModelNode;

class vtkCylinderWidget;

class VTK_OsteoPlan_EXPORT vtkOsteoPlanCuttingModelStep : public vtkOsteoPlanStep
{
public:
  static vtkOsteoPlanCuttingModelStep *New();
  vtkTypeRevisionMacro(vtkOsteoPlanCuttingModelStep,vtkOsteoPlanStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();
  virtual void HideUserInterface();
  virtual void TearDownGUI();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);  
  virtual void ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData) {};

  
  virtual void UpdateGUI();


protected:
  vtkOsteoPlanCuttingModelStep();
  ~vtkOsteoPlanCuttingModelStep();

  void AddGUIObservers();
  void RemoveGUIObservers();  

  bool ProcessingCallback;

  vtkKWFrame* MainFrame;

  //--------------------------------------------------
  //        Display Frame

  vtkKWFrameWithLabel* DisplayFrame;
  vtkKWPushButton* TogglePlaneButton;
  
  vtkBoxWidget2* CuttingPlane;
  vtkBoxRepresentation* CuttingPlaneRepresentation; 
  bool CutterAlreadyCreatedOnce;
  bool NextDisplayCutterStatus;

  //---------------------------------------------------


  //---------------------------------------------------
  //            Cut Frame

  vtkKWFrameWithLabel* CutFrame;
  vtkKWPushButton* ApplyCutButton;

  vtkSlicerNodeSelectorWidget* ModelToCutSelector;
  vtkMRMLModelNode* ModelToCut;

  bool ModelSelected;

  vtkCylinderWidget* cyl;
  //---------------------------------------------------

private:
  vtkOsteoPlanCuttingModelStep(const vtkOsteoPlanCuttingModelStep&);
  void operator=(const vtkOsteoPlanCuttingModelStep&);

  void CreateCutter();
  void ClipModel(vtkMRMLModelNode* model, vtkBoxWidget2* cuttingBox);
};

#endif
 
