/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkHybridProbeGUI_h
#define __vtkHybridProbeGUI_h

#ifdef WIN32
#include "vtkHybridProbeWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkHybridProbeLogic.h"

#define MAX_NUM_POINTS 3

class vtkKWPushButton;
class vtkSlicerNodeSelectorWidget;
class vtkMRMLLinearTransformNode;
class vtkMatrix4x4;

class VTK_HybridProbe_EXPORT vtkHybridProbeGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkHybridProbeGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkHybridProbeLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkHybridProbeGUI ( );
  virtual ~vtkHybridProbeGUI ( );

 private:
  vtkHybridProbeGUI ( const vtkHybridProbeGUI& ); // Not implemented.
  void operator = ( const vtkHybridProbeGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkHybridProbeGUI* New ();
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
  void BuildGUIForSelectingProbes();

  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();
  void RecordPoints();
  void PerformRegistration();
  void Reset();

 protected:
  
  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------
  
  int TimerFlag;
  int TimerInterval;

  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------

  vtkSlicerNodeSelectorWidget* EMTransformNodeSelector;
  vtkSlicerNodeSelectorWidget* OptTransformNodeSelector;

  vtkMRMLLinearTransformNode* EMTransformNode;
  vtkMRMLLinearTransformNode* OptTransformNode;

  vtkKWPushButton* ResetButton;
  vtkKWPushButton* RecordPointButton;
  vtkKWPushButton* PerformRegistrationButton;

  vtkMatrix4x4* EMTempMatrix;
  vtkMatrix4x4* OptTempMatrix;
  vtkMatrix4x4* EMPositions;
  vtkMatrix4x4* OptPositions;
  int numberOfPoints;
  bool firstClick;

  vtkMatrix4x4* EMOptRegMatrix;

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkHybridProbeLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

};



#endif
