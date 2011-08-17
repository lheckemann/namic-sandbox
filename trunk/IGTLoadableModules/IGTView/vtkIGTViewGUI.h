/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#ifndef __vtkIGTViewGUI_h
#define __vtkIGTViewGUI_h

#ifdef WIN32
#include "vtkIGTViewWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkIGTViewLogic.h"

class vtkKWPushButton;
class vtkKWCheckButton;
class vtkKWMenuButtonWithLabel;
class vtkMRMLSliceNode;
class vtkSlicerNodeSelectorWidget;
class vtkMRMLCrosshairNode;
class vtkMRMLCameraNode;
class vtkMRMLLinearTransformNode;

class VTK_IGTView_EXPORT vtkIGTViewGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkIGTViewGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkIGTViewLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  {
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private)
  //----------------------------------------------------------------

  vtkIGTViewGUI ( );
  virtual ~vtkIGTViewGUI ( );

 private:
  vtkIGTViewGUI ( const vtkIGTViewGUI& ); // Not implemented.
  void operator = ( const vtkIGTViewGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkIGTViewGUI* New ();
  void                  Init();
  virtual void          Enter ( );
  virtual void          Exit ( );
  void                  PrintSelf (ostream& os, vtkIndent indent );

  //----------------------------------------------------------------
  // Observer Management
  //----------------------------------------------------------------

  virtual void AddGUIObservers ( );
  virtual void RemoveGUIObservers ( );
  void         AddLogicObservers ( );
  void         RemoveLogicObservers ( );

  //----------------------------------------------------------------
  // Event Handlers
  //----------------------------------------------------------------

  virtual void ProcessLogicEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessGUIEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
  void         ProcessTimerEvents();
  void         HandleMouseEvent ( vtkSlicerInteractorStyle *style);
  static void  DataCallback ( vtkObject *caller, unsigned long eid, void *clientData, void *callData);

  //----------------------------------------------------------------
  // Build Frames
  //----------------------------------------------------------------

  virtual void BuildGUI();
  void BuildGUIForHelpFrame();
  void BuildGUIForViewers();
  void BuildGUIFor2DOverlay();

  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();

  void AddInplaneMenu();
  void RemoveInplaneMenu();

 protected:

  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------

  int TimerFlag;
  int TimerInterval;

  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------

  vtkMRMLCameraNode*          SceneCamera;
  vtkMRMLLinearTransformNode* TransformationNode;

  //------------------------------------------
  // Viewers

  vtkKWMenuButtonWithLabel* GreenViewerMenu;
  vtkKWMenuButtonWithLabel* YellowViewerMenu;
  vtkKWMenuButtonWithLabel* RedViewerMenu;
  vtkKWMenuButtonWithLabel* Viewer3DMenu;

  vtkMRMLSliceNode* SliceNodeRed;
  vtkMRMLSliceNode* SliceNodeYellow;
  vtkMRMLSliceNode* SliceNodeGreen;


  //------------------------------------------
  // Overlay

  vtkKWCheckButton*            crosshairButton;
  vtkSlicerNodeSelectorWidget* transformNodeSelector;
  vtkKWCheckButton*            trajectoryButton;
  vtkSlicerNodeSelectorWidget* trajectoryNodeSelector;

  bool RedObliqueReslicing;
  bool YellowObliqueReslicing;
  bool GreenObliqueReslicing;

  const char* RedReslicingType;
  const char* YellowReslicingType;
  const char* GreenReslicingType;




  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkIGTViewLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

};



#endif
