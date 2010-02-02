/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkPointRegistrationGUI_h
#define __vtkPointRegistrationGUI_h

#ifdef WIN32
#include "vtkPointRegistrationWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkPointRegistrationLogic.h"

class vtkKWPushButton;
class vtkKWEntryWithLabel;
class vtkKWEntry;
class vtkKWMultiColumnListWithScrollbars;
class vtkSlicerNodeSelectorWidget;
class vtkMatrix4x4;
class vtkMRMLLinearTransformNode;

class VTK_PointRegistration_EXPORT vtkPointRegistrationGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkPointRegistrationGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkPointRegistrationLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkPointRegistrationGUI ( );
  virtual ~vtkPointRegistrationGUI ( );

 private:
  vtkPointRegistrationGUI ( const vtkPointRegistrationGUI& ); // Not implemented.
  void operator = ( const vtkPointRegistrationGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkPointRegistrationGUI* New ();
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
  void BuildGUIForRegistrationFrame();
  void BuildGUIForRegistrationSelectFrame();
  void BuildGUIForApplyTransformFrame();

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

  vtkSlicerNodeSelectorWidget* PatTransformNode;
  vtkSlicerNodeSelectorWidget* TrackerNode;
  vtkKWEntryWithLabel *PatCoordinatesEntry;
  vtkKWEntryWithLabel *ImagCoordinatesEntry;
  vtkKWPushButton *GetPatCoordinatesButton;
  vtkKWPushButton *AddPointPairButton;
  vtkKWMultiColumnListWithScrollbars *PointPairMultiColumnList;
  vtkKWPushButton *DeletePointPairButton;
  vtkKWPushButton *DeleteAllPointPairButton;    
  vtkKWPushButton *RegisterButton;
  vtkKWPushButton *ResetButton;
  vtkKWPushButton *ApplyTransformButton;
  vtkSlicerNodeSelectorWidget* SelectRegNode;
  vtkKWPushButton* SelectRegNodeButton;
  
  
  //----------------------------------------------------------------
  // Variables
  //----------------------------------------------------------------
  vtkMatrix4x4* regTrans;
  vtkMRMLLinearTransformNode* regTransNode;

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkPointRegistrationLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

};



#endif
