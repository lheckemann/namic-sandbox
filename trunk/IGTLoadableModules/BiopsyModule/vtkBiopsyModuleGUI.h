/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkBiopsyModuleGUI_h
#define __vtkBiopsyModuleGUI_h

#ifdef WIN32
#include "vtkBiopsyModuleWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkBiopsyModuleLogic.h"

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
class vtkCollection;
class vtkActorCollection;

class VTK_BiopsyModule_EXPORT vtkBiopsyModuleGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkBiopsyModuleGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkBiopsyModuleLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkBiopsyModuleGUI ( );
  virtual ~vtkBiopsyModuleGUI ( );

 private:
  vtkBiopsyModuleGUI ( const vtkBiopsyModuleGUI& ); // Not implemented.
  void operator = ( const vtkBiopsyModuleGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkBiopsyModuleGUI* New ();
  void Init();
  virtual void Enter ( );
  virtual void Exit ( );
  void PrintSelf (ostream& os, vtkIndent indent );


  // Description: 
  // Get the categorization of the module.
  const char *GetCategory() const { return "IGT"; }

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
  void BuildGUIForBiopsyModule();

  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();
  void DrawPlanes(double angle_in_deg, double* center);

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
 
  vtkCollection* fiducialLists;
  vtkActorCollection* lineActors;
  vtkCollection* colorActors;

  vtkKWScale* translation;
  vtkKWScale* PlaneRotation;

  vtkMRMLLinearTransformNode* transformNode;
  vtkLineSource* lineBetweenFiducials;
  vtkKWRange* lineRange; 

  vtkKWEntryWithLabel* WholeRangeWidget;
  vtkKWPushButton* UpdateWholeRangeButton;

  vtkPlaneSource* PerpendicularPlane;
  vtkActor* planeActor;

  vtkPlaneSource* Plane0;
  vtkActor* plane0Actor;

  vtkPlaneSource* Plane90;
  vtkActor* plane90Actor;

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
  double normalVector1[3];
  double normalVector2[3];

  double sphereCenter[3];

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkBiopsyModuleLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

};



#endif
