/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#ifndef __vtkOsteoPlanGUI_h
#define __vtkOsteoPlanGUI_h

#ifdef WIN32
#include "vtkOsteoPlanWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkOsteoPlanLogic.h"

class vtkKWPushButton;
class vtkKWPushButtonSet;
class vtkKWWizardWidget;
class vtkOsteoPlanStep;
class vtkKWWizardStep;

class vtkMRMLOsteoPlanNode;

class vtkOsteoPlanCuttingModelStep;
class vtkOsteoPlanSelectingPartsStep;
class vtkOsteoPlanMovingPartsStep;
class vtkOsteoPlanPlacingScrewsStep;
class vtkOsteoPlanReturningOriginalPositionStep;
class vtkOsteoPlanDistractorStep;

class VTK_OsteoPlan_EXPORT vtkOsteoPlanGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkOsteoPlanGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkOsteoPlanLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  {
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private)
  //----------------------------------------------------------------

  vtkOsteoPlanGUI ( );
  virtual ~vtkOsteoPlanGUI ( );

 private:
  vtkOsteoPlanGUI ( const vtkOsteoPlanGUI& ); // Not implemented.
  void operator = ( const vtkOsteoPlanGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkOsteoPlanGUI* New ();
  void Init();
  virtual void Enter ( );
  virtual void Exit ( );
  void PrintSelf (ostream& os, vtkIndent indent );

  // Description:
  // Get the categorization of the module.
  const char *GetCategory() const { return "IGT"; }


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
  void BuildGUIForWorkflowFrame();
  void BuildGUIForWizardFrame();
  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();


  void PrepareMyStep(vtkOsteoPlanStep* wStep);
  void UpdateWorkflowStepNames();
  void ChangeWorkphaseGUI(int StepToGo);
  int GetStepNumber(vtkKWWizardStep* step);

  // Description:
  // Get wizard widget
  vtkGetObjectMacro(WizardWidget, vtkKWWizardWidget);
  vtkGetObjectMacro(OsteoPlanNode, vtkMRMLOsteoPlanNode);


 protected:

  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------

  int TimerFlag;
  int TimerInterval;
  vtkMRMLOsteoPlanNode* OsteoPlanNode;

  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------

  vtkKWPushButtonSet *WorkflowButtonSet;

  //----------------------------------------------------------------
  // Wizard Frame

  vtkSlicerModuleCollapsibleFrame *WizardFrame;
  vtkKWWizardWidget               *WizardWidget;

  //----------------------------------------------------------------
  // Wizard Steps
  //----------------------------------------------------------------

  vtkOsteoPlanCuttingModelStep                  *CuttingStep;
  vtkOsteoPlanSelectingPartsStep                *SelectingStep;
  vtkOsteoPlanMovingPartsStep                   *MovingStep;
  vtkOsteoPlanPlacingScrewsStep                 *PlacingStep;
  vtkOsteoPlanReturningOriginalPositionStep     *ReturningStep;
  vtkOsteoPlanDistractorStep                    *DistractorStep;

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkOsteoPlanLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

};



#endif
