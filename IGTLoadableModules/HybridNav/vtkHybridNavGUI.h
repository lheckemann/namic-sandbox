/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkHybridNavGUI_h
#define __vtkHybridNavGUI_h

#ifdef WIN32
#include "vtkHybridNavWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkHybridNavLogic.h"

class vtkKWPushButton;
class vtkSlicerNodeSelectorWidget;
class vtkKWEntry;
class vtkPivotCalibration;
class vtkKWMultiColumnListWithScrollbars;

class VTK_HybridNav_EXPORT vtkHybridNavGUI : public vtkSlicerModuleGUI
{
  //----------------------------------------------------------------
  // Type defines
  //----------------------------------------------------------------
 public:
  //BTX
  // Tool List update level options
  enum {
    UPDATE_SELECTED_ONLY   = 0,  // Update selected item only
    UPDATE_ALL      = 1,         // Update status for all items
  };
  //ETX
 public:

  vtkTypeRevisionMacro ( vtkHybridNavGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkHybridNavLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  {
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private)
  //----------------------------------------------------------------

  vtkHybridNavGUI ( );
  virtual ~vtkHybridNavGUI ( );

 private:
  vtkHybridNavGUI ( const vtkHybridNavGUI& ); // Not implemented.
  void operator = ( const vtkHybridNavGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkHybridNavGUI* New ();
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
  void BuildGUIForToolFrame();
  void BuildGUIForCalibrationFrame();

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

  //----------------------------------------------------------------
  //Tools Browser Frame

  vtkKWMultiColumnListWithScrollbars* ToolList;
  vtkKWPushButton*     AddToolButton;
  vtkKWPushButton*     DeleteToolButton;
  vtkKWEntry*          ToolNameEntry;
  vtkSlicerNodeSelectorWidget* ToolNodeSelectorMenu;
  vtkKWEntry*          ToolDescriptionEntry;

  //----------------------------------------------------------------
  //Pivot Calibration Frame

  vtkSlicerNodeSelectorWidget* CalibrationNodeSelectorMenu;
  vtkKWEntry* numPointsEntry;
  vtkKWPushButton* StartCalibrateButton;

  //----------------------------------------------------------------
  // Variables
  //----------------------------------------------------------------
  vtkPivotCalibration* pivot;

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkHybridNavLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

  //----------------------------------------------------------------
  // Connector and MRML Node list management
  //----------------------------------------------------------------
  //Populates the tool list with all the tool nodes
  void UpdateToolNodeList();
  void UpdateToolList(int updateLevel);
  void UpdateToolPropertyFrame(int i);

  //Vector with tool list


  //BTX
  // List of tool nodes
  std::vector<std::string> ToolNodeList;
  //ETX



};



#endif
