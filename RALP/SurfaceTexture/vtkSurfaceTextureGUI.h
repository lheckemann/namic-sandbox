/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkSurfaceTextureGUI_h
#define __vtkSurfaceTextureGUI_h

#ifdef WIN32
#include "vtkSurfaceTextureWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkSurfaceTextureLogic.h"

class vtkPolyDataNormals;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkActor;
class vtkRenderer;

class vtkKWPushButton;
class vtkSlicerNodeSelectorWidget;
class vtkKWRange;


class VTK_SurfaceTexture_EXPORT vtkSurfaceTextureGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkSurfaceTextureGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkSurfaceTextureLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkSurfaceTextureGUI ( );
  virtual ~vtkSurfaceTextureGUI ( );

 private:
  vtkSurfaceTextureGUI ( const vtkSurfaceTextureGUI& ); // Not implemented.
  void operator = ( const vtkSurfaceTextureGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkSurfaceTextureGUI* New ();
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
  void BuildGUIForTestFrame2();

  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();

  void ProcessWindowLevelCommand(double min, double max);
  void ProcessOuterLayerCommand(double min, double max);

  void SetInputVolumeID(const char* nodeID) { this->VolumeNodeID = nodeID; };
  void SetInputModelID(const char* nodeID) { this->ModelNodeID = nodeID; };

  void UpdateTexture();


 protected:
  
  //double TrilinearInterpolation(vtkImageData * spoints, double x[3]);
  double TrilinearInterpolation(vtkMRMLVolumeNode * vnode, double x[3]);

  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------
  
  int TimerFlag;
  int TimerInterval;

  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------

  vtkKWPushButton* TestButton11;
  vtkKWPushButton* TestButton12;
  vtkKWPushButton* TestButton21;
  vtkKWPushButton* TestButton22;

  vtkSlicerNodeSelectorWidget* ModelSelectorWidget;
  vtkSlicerNodeSelectorWidget* VolumeSelectorWidget;

  vtkKWRange * WindowLevelRange;
  vtkKWRange * OuterLayerRange;
  
  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkSurfaceTextureLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

  //BTX
  std::string VolumeNodeID;
  std::string ModelNodeID;
  //ETX


  vtkDoubleArray*     PointValue;
  vtkPolyDataNormals* PolyDataNormals;
  vtkPolyData*        PolyData;
  vtkPolyDataMapper*  Mapper;
  vtkActor*           Actor;
  vtkRenderer*        Renderer;

  int    NeedModelUpdate;
  double Lower;
  double Upper;
  double Step;
  double Window;
  double Level;

};



#endif
