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
class vtkMRMLLinearTransformNode;
class vtkLineSource;
class vtkKWRange;
class vtkKWEntryWithLabel;
class vtkPlaneSource;
class vtkKWCheckButtonWithLabel;
class vtkActor;
class vtkMatrix4x4;

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
  vtkKWScale* PlaneRotation;

  vtkMRMLLinearTransformNode* transformNode;
  vtkLineSource* lineBetweenFiducials;
  vtkKWRange* lineRange;

  vtkKWEntryWithLabel* WholeRangeWidget;
  vtkKWPushButton* UpdateWholeRangeButton;

  vtkPlaneSource* AxisPlane;
  vtkActor* planeActor;

  vtkKWCheckButtonWithLabel* togglePlaneVisibility;

  vtkMatrix4x4* transformMatrix;
  // Fiducial Positions
  double dpoint1[3];
  double dpoint2[3];

  // Center of the line between fiducials
  double lineCenter[3];

  // Vectors from P1 and P2 to the center
  double P1Vector[3];
  double P2Vector[3];

  // Length of these vector (same distance between P1 and center and P2 and center)
  double PVectorLength;

  // Normalized vector 
  double P1VectorNormalized[3];
  double P2VectorNormalized[3];

  // New point of the line after sliding
  double lineTip1[3];
  double lineTip2[3];

  // Normal vector to the line
  double normalVector[3];

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkLineMotionLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

};



#endif
