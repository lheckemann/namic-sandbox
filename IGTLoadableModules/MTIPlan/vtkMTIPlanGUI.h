/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#ifndef __vtkMTIPlanGUI_h
#define __vtkMTIPlanGUI_h

#ifdef WIN32
#include "vtkMTIPlanWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkMTIPlanLogic.h"

#include "vtkKWLoadSaveButton.h"
#include "vtkKWLoadSaveButtonWithLabel.h"
#include "vtkCollection.h"

class vtkKWPushButton;
class vtkKWLoadSaveButtonWithLabel;
class vtkCollection;
class vtkMTIPlanLogic;

class VTK_MTIPlan_EXPORT vtkMTIPlanGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkMTIPlanGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkMTIPlanLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  {
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private)
  //----------------------------------------------------------------

  vtkMTIPlanGUI ( );
  virtual ~vtkMTIPlanGUI ( );

 private:
  vtkMTIPlanGUI ( const vtkMTIPlanGUI& ); // Not implemented.
  void operator = ( const vtkMTIPlanGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkMTIPlanGUI* New ();
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
  void BuildGUIForLoadingTrajectory();

  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();

  //BTX
  void DrawTrajectories(std::vector<vtkMTIPlanLogic::Trajectory> vTraj);
  //ETX

 protected:

  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------

  int TimerFlag;
  int TimerInterval;

  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------

  vtkKWLoadSaveButtonWithLabel* SelectXMLFileButton;
  vtkCollection* TrajectoryCollection;

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkMTIPlanLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

};



#endif
