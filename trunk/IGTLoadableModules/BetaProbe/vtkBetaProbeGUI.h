/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#ifndef __vtkBetaProbeGUI_h
#define __vtkBetaProbeGUI_h

#ifdef WIN32
#include "vtkBetaProbeWin32Header.h"
#endif

#include <iostream>
#include <fstream>

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkBetaProbeLogic.h"

#include "vtkMRMLUDPServerNode.h"

class vtkKWPushButton;
class vtkSlicerNodeSelectorWidget;
class vtkKWLabel;
class vtkKWFileBrowserDialog;

class vtkMRMLLinearTransformNode;
class vtkMRMLScalarVolumeNode;
class vtkSlicerNodeSelectorWidget;
class vtkMatrix4x4;
class vtkCollection;
class vtkKWRadioButton;
class vtkKWEntry;

class VTK_BetaProbe_EXPORT vtkBetaProbeGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkBetaProbeGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkBetaProbeLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  {
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

  const char *GetCategory() const { return "IGT";}

  void SetContinuousMode(bool val){this->continuous_mode = val;};
  bool GetContinuousMode(){return this->continuous_mode;};

  void Capture_Data();

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private)
  //----------------------------------------------------------------

  vtkBetaProbeGUI ( );
  virtual ~vtkBetaProbeGUI ( );

 private:
  vtkBetaProbeGUI ( const vtkBetaProbeGUI& ); // Not implemented.
  void operator = ( const vtkBetaProbeGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkBetaProbeGUI* New ();
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
  void BuildGUIForInputs();
  void BuildGUIForDataRecorder();
  void BuildGUIForThresholdDetection();
  void BuildGUIForMapping();

  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();
  void CenterImage(vtkMRMLVolumeNode *volumeNode);


 protected:

  //----------------------------------------------------------------
  // File
  //----------------------------------------------------------------

  ofstream BetaProbeCountsWithTimestamp;

  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------

  int TimerFlag;
  int TimerInterval;

  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------

  vtkKWPushButton*              Capture;
  vtkKWPushButton*              Start_Button;
  vtkKWPushButton*              Stop_Button;
  vtkKWPushButton*              SelectFile;
  vtkKWPushButton*              CloseFile;
  vtkSlicerNodeSelectorWidget*  CounterNode;
  vtkSlicerNodeSelectorWidget*  TrackerNode;
  vtkKWLabel*                   Capture_status;
  vtkKWFileBrowserDialog*       FileSelector;

  vtkMRMLUDPServerNode*         Counts;
  vtkMRMLLinearTransformNode*   Probe_Position;
  vtkMatrix4x4*                 Probe_Matrix;

  vtkKWRadioButton      *RadioBackgroundButton;
  vtkKWEntry            *BackgroundValueEntry;
  vtkKWPushButton       *BackgroundAcceptButton;
  vtkKWRadioButton      *RadioTumorButton;
  vtkKWEntry            *TumorValueEntry;
  vtkKWPushButton       *TumorAcceptButton;
  vtkKWRadioButton      *RadioThresholdButton;
  vtkKWEntry            *ThresholdValueEntry;
  vtkKWPushButton       *StartDetectionButton;

  vtkSlicerNodeSelectorWidget *DataSelector;
  vtkMRMLScalarVolumeNode     *DataToMap;
  vtkKWPushButton             *MappingButton;
  vtkKWLabel                  *LabelStatus;

  int   ThresholdTumorDetection;
  bool  BackgroundAccepted;
  bool  TumorAccepted;

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkBetaProbeLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

  bool continuous_mode;
};



#endif
