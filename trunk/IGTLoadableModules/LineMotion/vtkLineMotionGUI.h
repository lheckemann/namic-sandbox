/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkLineMotionGUI_h
#define __vtkLineMotionGUI_h

#ifdef WIN32
#include "vtkLineMotionWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkLineMotionLogic.h"

class vtkKWPushButton;
class vtkSlicerNodeSelectorWidget;
class vtkMRMLFiducialListNode;
class vtkKWScale;
class vtkTransform;
class vtkMRMLLinearTransformNode;
class vtkLineSource;
class vtkKWRange;

class VTK_LineMotion_EXPORT vtkLineMotionGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkLineMotionGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkLineMotionLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkLineMotionGUI ( );
  virtual ~vtkLineMotionGUI ( );

 private:
  vtkLineMotionGUI ( const vtkLineMotionGUI& ); // Not implemented.
  void operator = ( const vtkLineMotionGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkLineMotionGUI* New ();
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
  void BuildGUIForLineMotion();

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

  vtkSlicerNodeSelectorWidget* fiducialListWidget;
  vtkMRMLFiducialListNode* fiducialListNode;
  vtkKWPushButton* drawline;

  vtkKWScale* translation;

  vtkTransform* transformation;
  vtkMRMLLinearTransformNode* transformNode;
  vtkLineSource* lineBetweenFiducials;
  vtkKWRange* lineRange;

  // Distance between the two fiducials
  double lineLength;
  
  // Fiducial Positions
  double dpoint1[3];
  double dpoint2[3];

  // Direction of the axis
  double vector_normalized[3];
  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkLineMotionLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

};



#endif
