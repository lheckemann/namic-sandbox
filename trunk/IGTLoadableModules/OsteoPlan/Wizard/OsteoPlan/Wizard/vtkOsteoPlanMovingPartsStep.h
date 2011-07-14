/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkOsteoPlanMovingPartsStep_h
#define __vtkOsteoPlanMovingPartsStep_h

#include "vtkOsteoPlanStep.h"

class vtkSlicerInteractorStyle;
class vtkKWEntryWithLabel;
class vtkKWFrame;
class vtkKWFrameWithLabel;
class vtkKWLabel;
class vtkKWPushButton;
class vtkSlicerNodeSelectorWidget;
class vtkMRMLModelNode;
class vtkKWScaleWithLabel;
class vtkMRMLLinearTransformNode;
class vtkSlicerTransformEditorWidget;

class VTK_OsteoPlan_EXPORT vtkOsteoPlanMovingPartsStep : public vtkOsteoPlanStep
{
public:
  static vtkOsteoPlanMovingPartsStep *New();
  vtkTypeRevisionMacro(vtkOsteoPlanMovingPartsStep,vtkOsteoPlanStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();
  virtual void HideUserInterface();
  virtual void TearDownGUI();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);  
  virtual void ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData) {};
 
  virtual void UpdateGUI();

  void HandleMouseEvent(vtkSlicerInteractorStyle* style);

protected:
  vtkOsteoPlanMovingPartsStep();
  ~vtkOsteoPlanMovingPartsStep();

  void AddGUIObservers();
  void RemoveGUIObservers();  

  bool ProcessingCallback;
 
  vtkKWFrame* MainFrame;
  vtkKWFrameWithLabel* SelectingPartToMove;
  vtkKWLabel* ModelToMoveLabel;
  vtkSlicerNodeSelectorWidget* ModelToMoveSelector;
  vtkMRMLModelNode* ModelToMove;
  //  vtkKWEntryWithLabel* PartNameEntry;
  vtkKWPushButton* StartMovingButton;

  bool StartMoving;

  vtkKWFrameWithLabel* MovingFrame;
  /*
  vtkKWFrameWithLabel* TranslationFrame;
  vtkKWFrameWithLabel* RotationFrame;

  vtkKWScaleWithLabel* XTranslationScale; 
  vtkKWScaleWithLabel* YTranslationScale;
  vtkKWScaleWithLabel* ZTranslationScale;

  vtkKWScaleWithLabel* XRotationScale;
  vtkKWScaleWithLabel* YRotationScale;
  vtkKWScaleWithLabel* ZRotationScale;
  */
  vtkSlicerTransformEditorWidget* TransformationEditor;

  vtkMRMLLinearTransformNode* ParentTransformationNode;


private:
  vtkOsteoPlanMovingPartsStep(const vtkOsteoPlanMovingPartsStep&);
  void operator=(const vtkOsteoPlanMovingPartsStep&);

};

#endif
 
