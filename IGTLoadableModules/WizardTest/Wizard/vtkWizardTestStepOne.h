/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkWizardTestStepOne_h
#define __vtkWizardTestStepOne_h

#include "vtkWizardTestStep.h"

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

class VTK_WizardTest_EXPORT vtkWizardTestStepOne : public vtkWizardTestStep
{
public:
  static vtkWizardTestStepOne *New();
  vtkTypeRevisionMacro(vtkWizardTestStepOne,vtkWizardTestStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();
  virtual void HideUserInterface();
  virtual void TearDownGUI();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);  
  virtual void ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData) {};

  
  virtual void UpdateGUI();

protected:
  vtkWizardTestStepOne();
  ~vtkWizardTestStepOne();

  void AddGUIObservers();
  void RemoveGUIObservers();  

  bool ProcessingCallback;

  vtkKWFrame *MainFrame;
  vtkKWPushButton* LoadMe;
  
private:
  vtkWizardTestStepOne(const vtkWizardTestStepOne&);
  void operator=(const vtkWizardTestStepOne&);
};

#endif
 
