/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkBetaProbeNavGUI_h
#define __vtkBetaProbeNavGUI_h

#ifdef WIN32
#include "vtkBetaProbeNavWin32Header.h"
#endif

#include <iostream>
#include <fstream>

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkBetaProbeNavLogic.h"

class vtkKWPushButton;
class vtkKWCheckButton;
class vtkSlicerNodeSelectorWidget;
class vtkKWRadioButtonSet;
class vtkMRMLModelNode;
class vtkKWEntry;
class vtkMRMLScalarVolumeNode;

class VTK_BetaProbeNav_EXPORT vtkBetaProbeNavGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkBetaProbeNavGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkBetaProbeNavLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  ofstream BetaProbeCountsWithTimestamp; 
  vtkBetaProbeNavGUI ( );
  virtual ~vtkBetaProbeNavGUI ( );

 private:
  vtkBetaProbeNavGUI ( const vtkBetaProbeNavGUI& ); // Not implemented.
  void operator = ( const vtkBetaProbeNavGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkBetaProbeNavGUI* New ();
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
  void ProcessTimerEvents();
  void HandleMouseEvent(vtkSlicerInteractorStyle *style);
  static void DataCallback(vtkObject *caller, 
                           unsigned long eid, void *clientData, void *callData);
  
  //----------------------------------------------------------------
  // Build Frames
  //----------------------------------------------------------------

  virtual void BuildGUI ( );
  void BuildGUIForHelpFrame();
  void BuildGUIForImportDataFrame();

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

  vtkKWPushButton* StartButton;
  vtkKWPushButton* StopButton;
  vtkSlicerNodeSelectorWidget* TransformNode;
  vtkSlicerNodeSelectorWidget* CountNode;
  vtkSlicerNodeSelectorWidget* ImageNode;
  vtkSlicerNodeSelectorWidget* ModelNode;
  vtkKWRadioButtonSet* ModelTypeButtonSet;
  vtkKWRadioButtonSet* ImageTypeButtonSet;
  vtkKWRadioButtonSet* DataTypeButtonSet;
  vtkKWCheckButton* ImageCheckButton;
  vtkKWEntry* RangeEntry;
  vtkKWEntry* HeightEntry;
  vtkKWEntry* REntry;
  vtkKWEntry* AEntry;
  vtkKWEntry* SEntry;
  vtkKWEntry* ProbeDiameterEntry;
  vtkKWEntry* UpdateEntry;
  vtkKWRadioButtonSet* DataEntryButtonSet;
  vtkKWPushButton* DataCaptureButton;
  vtkKWEntry* StartTimeEntry;

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkBetaProbeNavLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;
  
  //----------------------------------------------------------------
  // Variables
  //----------------------------------------------------------------
  int ManualDataCapture;

  //----------------------------------------------------------------
  // Model Nodes
  //----------------------------------------------------------------
  vtkMRMLModelNode* mnode;
  vtkMRMLScalarVolumeNode* scalnode;

};

#endif
