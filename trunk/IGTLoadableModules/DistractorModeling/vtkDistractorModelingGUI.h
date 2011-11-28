/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#ifndef __vtkDistractorModelingGUI_h
#define __vtkDistractorModelingGUI_h

#ifdef WIN32
#include "vtkDistractorModelingWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkKWScale.h"
#include "vtkMRMLModelNode.h"
#include "vtkMatrix4x4.h"

#include "vtkDistractorModelingLogic.h"

class vtkKWPushButton;
class vtkSlicerNodeSelectorWidget;
class vtkKWScale;
class vtkMRMLModelNode;
class vtkMatrix4x4;

class VTK_DistractorModeling_EXPORT vtkDistractorModelingGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkDistractorModelingGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkDistractorModelingLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  {
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private)
  //----------------------------------------------------------------

  vtkDistractorModelingGUI ( );
  virtual ~vtkDistractorModelingGUI ( );

 private:
  vtkDistractorModelingGUI ( const vtkDistractorModelingGUI& ); // Not implemented.
  void operator = ( const vtkDistractorModelingGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkDistractorModelingGUI* New ();
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
  void BuildGUIForTestFrame1();

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

  vtkSlicerNodeSelectorWidget* RailSelector;
  vtkSlicerNodeSelectorWidget* SliderSelector;
  vtkSlicerNodeSelectorWidget* PistonSelector;
  vtkSlicerNodeSelectorWidget* CylinderSelector;

  vtkMRMLModelNode* RailModel;
  vtkMRMLModelNode* SliderModel;
  vtkMRMLModelNode* PistonModel;
  vtkMRMLModelNode* CylinderModel;

  vtkMRMLLinearTransformNode* SliderTransformNode;
  vtkMRMLLinearTransformNode* PistonTransformNode;
  vtkMRMLLinearTransformNode* CylinderTransformNode;

  vtkKWScale* MovingScale;

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkDistractorModelingLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

};



#endif
