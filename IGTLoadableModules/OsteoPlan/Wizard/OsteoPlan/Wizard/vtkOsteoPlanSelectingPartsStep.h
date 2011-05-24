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

class vtkKWEntry;
class vtkKWEntrySet;
class vtkKWEntryWithLabel;
class vtkKWFrame;
class vtkKWLabel;
class vtkSlicerNodeSelectorWidget;
class vtkKWMatrixWidgetWithLabel;
class vtkKWMenuButton;
class vtkKWMenuButtonWithLabel;
class vtkKWMultiColumnList;
class vtkKWMultiColumnListWithScrollbars;
class vtkKWPushButton;
class vtkKWText;
class vtkImageData;
class vtkMRMLScalarVolumeNode;
class vtkMRMLSelectionNode;
class vtkMRMLFiducialListNode;
class vtkKWCheckButton;

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

protected:
  vtkOsteoPlanSelectingPartsStep();
  ~vtkOsteoPlanSelectingPartsStep();

  void AddGUIObservers();
  void RemoveGUIObservers();  

  bool ProcessingCallback;

  vtkKWFrame *MainFrame;
  
private:
  vtkOsteoPlanSelectingPartsStep(const vtkOsteoPlanSelectingPartsStep&);
  void operator=(const vtkOsteoPlanSelectingPartsStep&);
};

#endif
 
