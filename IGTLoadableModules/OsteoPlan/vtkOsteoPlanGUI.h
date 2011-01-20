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
class vtkKWCheckButtonWithLabel;
class vtkSlicerNodeSelectorWidget;
class vtkKWPushButton;
class vtkBoxWidget;

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
  void ProcessTimerEvents();
  void HandleMouseEvent(vtkSlicerInteractorStyle *style);
  static void DataCallback(vtkObject *caller, 
                           unsigned long eid, void *clientData, void *callData);

  
  //----------------------------------------------------------------
  // Build Frames
  //----------------------------------------------------------------

  virtual void BuildGUI ( );
  void BuildGUIForHelpFrame();
  void BuildGUICutter();

  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();


 protected:
  

  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------
  
  int TimerFlag;
  int TimerInterval;

  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------

  vtkKWCheckButtonWithLabel* EnableCutter;


  //----------------------------------------------------------------
  // Plane widgets
  //----------------------------------------------------------------
 
  vtkBoxWidget* CuttingPlane;
  vtkSlicerNodeSelectorWidget* ModelToCutSelector;
  vtkKWPushButton* PerformCutButton;

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkOsteoPlanLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

};



#endif
