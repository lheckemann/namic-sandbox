/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkVirtualFixtureGUI_h
#define __vtkVirtualFixtureGUI_h

#ifdef WIN32
#include "vtkVirtualFixtureWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkVirtualFixtureLogic.h"

class vtkKWPushButton;
class vtkSphereSource;

class VTK_VirtualFixture_EXPORT vtkVirtualFixtureGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkVirtualFixtureGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkVirtualFixtureLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

  //BTX
  typedef struct {
    std::string       name;
    double            center[3];
    double            radius;
    vtkMRMLModelNode* model;
    vtkSphereSource*  sphere;
  } SphereData;
  //ETX

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkVirtualFixtureGUI ( );
  virtual ~vtkVirtualFixtureGUI ( );

 private:
  vtkVirtualFixtureGUI ( const vtkVirtualFixtureGUI& ); // Not implemented.
  void operator = ( const vtkVirtualFixtureGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkVirtualFixtureGUI* New ();
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
  void BuildGUIForSphereControl();
  //void BuildGUIForTestFrame1();
  //void BuildGUIForTestFrame2();

  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();


  //----------------------------------------------------------------
  // Sphere controls
  //----------------------------------------------------------------

  int AddNewSphere(const char* name);
  int SelectSphere(int n);
  int UpdateSphere();
  void AddSphereModel(SphereData* data, double center[3], double radius,
                      double color[3]);
  void UpdateSphereModel(SphereData* data);
  void HighlightSphereModel(SphereData* data, bool highlight);

 protected:
  
  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------
  
  int TimerFlag;
  int TimerInterval;

  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------

  vtkKWMenuButton* SphereMenu;
  vtkKWEntry*      SphereNameEntry;
  vtkKWEntry*      CenterXEntry;
  vtkKWEntry*      CenterYEntry;
  vtkKWEntry*      CenterZEntry;
  vtkKWEntry*      RadiusEntry;
  vtkKWPushButton* UpdateSphereButton;
  vtkKWPushButton* DeleteSphereButton;

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkVirtualFixtureLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

  //----------------------------------------------------------------
  // Sphere controls
  //----------------------------------------------------------------

  //BTX
  typedef std::vector<SphereData> SphereListType;
  SphereListType SphereList;

  int CurrentSphere;
  //ETX
  
};



#endif
