/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkWizardTestGUI_h
#define __vtkWizardTestGUI_h

#ifdef WIN32
#include "vtkWizardTestWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkWizardTestLogic.h"

class vtkKWPushButton;
class vtkKWPushButtonSet;
class vtkKWWizardWidget;
class vtkWizardTestStep;
class vtkKWWizardStep;

class VTK_WizardTest_EXPORT vtkWizardTestGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkWizardTestGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkWizardTestLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkWizardTestGUI ( );
  virtual ~vtkWizardTestGUI ( );

 private:
  vtkWizardTestGUI ( const vtkWizardTestGUI& ); // Not implemented.
  void operator = ( const vtkWizardTestGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkWizardTestGUI* New ();
  void Init();
  virtual void Enter ( );
  virtual void Exit ( );
  void PrintSelf (ostream& os, vtkIndent indent );

  //----------------------------------------------------------------
  // Observer Management
  //----------------------------------------------------------------

  virtual void AddGUIObservers ( );
  virtual void RemoveGUIObservers ( );
  void AddLogicObservers ( );
  void RemoveLogicObservers ( );

  //----------------------------------------------------------------
  // Event Handlers
  //----------------------------------------------------------------

  virtual void ProcessLogicEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessGUIEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void TearDownGUI();
  void ProcessTimerEvents();
  void HandleMouseEvent(vtkSlicerInteractorStyle *style);
  static void DataCallback(vtkObject *caller, 
                           unsigned long eid, void *clientData, void *callData);
  
  //----------------------------------------------------------------
  // Build Frames
  //----------------------------------------------------------------

  virtual void BuildGUI ( );
  void BuildGUIForHelpFrame();
  void BuildGUIForTestFrame1();
  void BuildGUIForWizardFrame();
  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();


  void AddButtons();
  void AddMyStep(vtkWizardTestStep* wStep);
  void UpdateWorkflowStepNames();
  void ChangeWorkphaseGUI(int StepToGo);
  int GetStepNumber(vtkKWWizardStep* step);

  // Description: 
  // Get wizard widget
  vtkGetObjectMacro(WizardWidget, vtkKWWizardWidget);



 protected:
  
  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------
  
  int TimerFlag;
  int TimerInterval;

  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------

  vtkKWPushButtonSet* WorkflowButtonSet;
  
  //----------------------------------------------------------------
  // Wizard Frame

  vtkSlicerModuleCollapsibleFrame *WizardFrame;
  vtkKWWizardWidget *WizardWidget;

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkWizardTestLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

};



#endif
