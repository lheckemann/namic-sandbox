/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkWizardTestStepTwo_h
#define __vtkWizardTestStepTwo_h

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

class VTK_WizardTest_EXPORT vtkWizardTestStepTwo : public vtkWizardTestStep
{
public:
  static vtkWizardTestStepTwo *New();
  vtkTypeRevisionMacro(vtkWizardTestStepTwo,vtkWizardTestStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();
  virtual void HideUserInterface();
  virtual void TearDownGUI();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);  
  virtual void ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData) {};

  
  virtual void UpdateGUI();

protected:
  vtkWizardTestStepTwo();
  ~vtkWizardTestStepTwo();

  void AddGUIObservers();
  void RemoveGUIObservers();  

  bool ProcessingCallback;

  vtkKWFrame *MainFrame;
  
private:
  vtkWizardTestStepTwo(const vtkWizardTestStepTwo&);
  void operator=(const vtkWizardTestStepTwo&);
};

#endif
 
